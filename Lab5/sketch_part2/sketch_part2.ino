#include "concurrency_func.h"
#include "condition.h"

lock_t *serial_lock;
lock_t *book_lock;
// readers can only read when there are no writers
// multiple readers can read the book
lock_t *users_lock;
cond_t *no_writers;
cond_t *no_users;
int book = 0;
int nw = 0;
int nr = 0;

// writer
void p1() {
  lock_acquire(users_lock);
  delay(400);
  if(nw > 0) {
    Serial.println("P1 waiting");
    cond_wait(users_lock, no_users);
  }
  nw++;
  lock_release(users_lock);
  delay(1000);
  lock_acquire(book_lock);
  Serial.println("P1: stocking books");
  Serial.println(book);
  book++;
  Serial.println("Changed the book");
  delay(200);
  lock_release(book_lock);
  lock_acquire(users_lock);
  nw--;
  int waiting = cond_waiting(users_lock, no_users);
//  Serial.print("waiting: ");
  Serial.println(waiting);
  if(nw == 0 && waiting) {
    Serial.println("Another process waiting");
    cond_signal(users_lock, no_users);
  } else {
    Serial.println("No one else waiting");
    lock_release(users_lock);
  }
  return;
}

// another reader
void p2() {
  delay(10);
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
  return;
}

// another reader
void p3() {
  delay(10);
  lock_acquire(users_lock);
  if(nw > 0) {
    Serial.println("P3 waiting");
    cond_wait(users_lock, no_users);
  }
  nw++;
  lock_release(users_lock);
  lock_acquire(book_lock);
  Serial.print("P3: reading book");
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
  lock_acquire(serial_lock);
  Serial.println("in reader entry");
  lock_release(serial_lock);
  lock_acquire(users_lock);
  if(nw > 0) {
    lock_acquire(serial_lock);
    Serial.println("nw queue");
    lock_release(serial_lock);
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
    lock_acquire(serial_lock);
    Serial.println("nu queue");
    lock_release(serial_lock);
    cond_wait(users_lock, no_users);
  }
  nw++;
  lock_release(users_lock);
}

void writer_exit() {
  lock_acquire(users_lock);
  nw--;
  if (nw == 0 && cond_waiting(users_lock, no_writers)) {
    lock_acquire(serial_lock);
    Serial.print("No writers");
    lock_release(serial_lock);
    cond_signal(users_lock, no_writers); 
  } else if(nr == 0 && nw == 0 && cond_waiting(users_lock, no_users)) {
    lock_acquire(serial_lock);
//    Serial.print("No users");
    lock_release(serial_lock);
    cond_signal(users_lock, no_users);
  } else {
    lock_acquire(serial_lock);
    Serial.print("no one waiting");
    lock_release(serial_lock);
    lock_release(users_lock);
  }
}

void writer() {
  while(1) {
    if(serialEventRun) serialEventRun();
    writer_entry();
    lock_acquire(book_lock);
    lock_acquire(serial_lock);
    Serial.println("Writer modifying");
    lock_release(serial_lock);
    book++;
    delay(200);
    lock_release(book_lock);
    writer_exit();
  }
  delay(10000);
}

void reader() {
  while(1) {
    // print statements to see if in reader
    lock_acquire(serial_lock);
    Serial.print("in reader");
    lock_release(serial_lock);

    // help serial port run
    if(serialEventRun) serialEventRun();
    // enter reader entry to check if there are any writers
    reader_entry();
    
    // read from the book variable
    lock_acquire(book_lock);
    lock_acquire(serial_lock);
    Serial.print("Reader reading ");
    Serial.println(book);
    lock_release(serial_lock);
    lock_release(book_lock);
    
    // decrement nr and signal any waiting writers
    reader_exit();
    
    delay(100);
  }
}

int test1_setup() {
  Serial.begin(9600);
  serial_lock = (lock_t*) malloc(sizeof(lock_t));
  lock_init(serial_lock);
  book_lock = (lock_t*) malloc(sizeof(lock_t));
  lock_init(book_lock);
  users_lock = (lock_t*) malloc(sizeof(lock_t));
  lock_init(users_lock);
  no_users = (cond_t*) malloc(sizeof(cond_t));
  cond_init(users_lock, no_users);
  no_writers = (cond_t*) malloc(sizeof(cond_t));
  cond_init(users_lock, no_writers);
  if(process_create(reader, 64) < 0) {
    return 0;
  }
  if(process_create(writer, 64) < 0) {
    return 0;
  }
  return 1;
}

int test2_setup() {
  Serial.begin(9600);
  book_lock = (lock_t*)malloc(sizeof(lock_t));
  lock_init(book_lock);
  users_lock = (lock_t*)malloc(sizeof(lock_t));
  lock_init(users_lock);
  no_users = (cond_t*)malloc(sizeof(cond_t));
  cond_init(users_lock, no_users);
  if(process_create(p1, 128) < 0) {
    return 0;
  }
  if(process_create(p2, 128) < 0) {
    return 0;
  }
  if(process_create(p3, 128) < 0) {
    return 0;
  }
  return 1;
}

void setup() {
  if(!test2_setup()) {
    return;
  }
}

void loop() {
  process_start();
  while (1) {
    Serial.println("spinning");
    delay(1000);
  }
}
