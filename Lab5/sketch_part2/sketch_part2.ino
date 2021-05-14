#include "concurrency_func.h"
#include "condition.h"

lock_t *book_lock;
// readers can only read when there are no writers
// multiple readers can read the book
lock_t *users_lock;
cond_t *no_writers;
cond_t *no_users;
char* book = "This is a book.";
int nw = 0;
int nr = 0;

// writer
void p1() {
  delay(400);
  lock_acquire(users_lock);
  if(nw > 0) {
//    Serial.println("P1 waiting");
    cond_wait(users_lock, no_users);
  }
  nw++;
  lock_release(users_lock);
  delay(1000);
  lock_acquire(book_lock);
  Serial.println("P1: changing the book");
  Serial.println(book);
  book = "This is a new book.";
  Serial.println("Changed the book");
  lock_release(book_lock);
  lock_acquire(users_lock);
  nw--;
  if(nw == 0 && cond_waiting(users_lock, no_users)) {
    Serial.println("Another process waiting");
    cond_signal(users_lock, no_users);
  } else {
    Serial.println("No one else waiting");
    lock_release(users_lock);
  }
}

// another reader
void p2() {
  lock_acquire(users_lock);
  if(nw > 0) {
    Serial.println("P2 waiting");
    cond_wait(users_lock, no_users);
  }
  nw++;
  lock_release(users_lock);
  lock_acquire(book_lock);
  Serial.print("P2: reading book");
//  book = "This is a new new P2 book";
  Serial.println(book);
  lock_release(book_lock);
  lock_acquire(users_lock);
  nw--;
  if(nw == 0 && cond_waiting(users_lock, no_users)) {
    cond_signal(users_lock, no_users);
  } else {
    lock_release(users_lock);
  }
}

void reader_entry() {
  lock_acquire(users_lock);
  if(nw > 0) {
    cond_wait(users_lock, no_writers);
  }
  nr++;
  lock_release(users_lock);
}

void reader_exit() {
  lock_acquire(users_lock);
  nr--;
  if(nr == 0 && nw == 0 && cond_waiting(users_lock, no_users)) {
    cond_signal(users_lock, no_users);
  } else {
    lock_release(users_lock);
  }
}

void writer_entry(){
  lock_acquire(users_lock);
  if(nr > 0 || nw > 0) {
    cond_wait(users_lock, no_users);
  }
  nw++;
  lock_release(users_lock);
}

void writer_exit() {
  lock_acquire(users_lock);
  if(nr == 0 && nw == 0 && cond_waiting(users_lock, no_users)) {
    cond_signal(users_lock, no_users);
  } else if (nw == 0 && cond_waiting(users_lock, no_writers)) {
    cond_signal(users_lock, no_writers); 
  } else {
    lock_release(users_lock);
  }
}

void setup() {
  Serial.begin(9600);
  book_lock = (lock_t*)malloc(sizeof(lock_t));
  lock_init(book_lock);
  users_lock = (lock_t*)malloc(sizeof(lock_t));
  lock_init(users_lock);
  no_users = (cond_t*)malloc(sizeof(cond_t));
  cond_init(users_lock, no_users);
  if(process_create(p1, 128) < 0) {
    return;
  }
  if(process_create(p2, 128) < 0) {
    return;
  }
//  if(process_create(p3, 64) < 0) {
//    return;
//  }

}

void loop() {
  process_start();
  while (1) {
    Serial.println("spinning");
    delay(1000);
  }
}
