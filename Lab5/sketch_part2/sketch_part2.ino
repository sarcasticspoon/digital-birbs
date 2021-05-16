#include "concurrency_func.h"
#include "condition.h"

lock_t *serial_lock;
lock_t *book_lock;
// readers can only read when there are no writers
// multiple readers can read the book
lock_t *users_lock;
cond_t *no_writers;
cond_t *no_users;
volatile int book = 0;
volatile int nw = 0;
volatile int nr = 0;

// writer
void p1() {
  // acquire lockf for nw/nr
  lock_acquire(users_lock);
  delay(400);
  // wait on cv if there are other users
  if(nw > 0) {
    Serial.println("P1 waiting");
    cond_wait(users_lock, no_users);
  }
  // increment number of users
  nw++;
  lock_release(users_lock);
  delay(1000);
  // modify book variable
  lock_acquire(book_lock);
  Serial.println("P1: stocking books");
  Serial.println(book);
  book++;
  Serial.println("Changed the book");
  delay(200);
  lock_release(book_lock);
  // decrement number of users
  lock_acquire(users_lock);
  nw--;
  // check to see if anyone else is waiting
  int waiting = cond_waiting(users_lock, no_users);
  if(nw == 0 && waiting) {
    Serial.println("Another process waiting");
    cond_signal(users_lock, no_users);
  } else {
    Serial.println("No one else waiting");
    lock_release(users_lock);
  }
  return;
}

// reader
void p2() {
  delay(10);
  // check to see if anyone else is using the book variable
  // if so, wait on cv
  lock_acquire(users_lock);
  if(nw > 0) {
    Serial.println("P2 waiting");
    cond_wait(users_lock, no_users);
  }
  
  // increment number of users
  nw++;
  lock_release(users_lock);
  // read from the book variable
  lock_acquire(book_lock);
  Serial.print("P2: reading book");
  Serial.println(book);
  lock_release(book_lock);
  
  // decrement number of users
  lock_acquire(users_lock);
  nw--;
  // if others are waiting on condition signal
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
  // check to see if anyone else is using the book variable
  // if so, wait on cv
  lock_acquire(users_lock);
  if(nw > 0) {
    Serial.println("P3 waiting");
    cond_wait(users_lock, no_users);
  }
  // increment number of users
  nw++;
  lock_release(users_lock);

  // read from book variable
  lock_acquire(book_lock);
  Serial.print("P3: reading book");
  Serial.println(book);
  lock_release(book_lock);

  // decrement number of writers
  lock_acquire(users_lock);
  nw--;
  // signal other processes waiting on cv if they exist
  if(nw == 0 && cond_waiting(users_lock, no_users)) {
    cond_signal(users_lock, no_users);
  } else {
    lock_release(users_lock);
  }
}

// checks to see if other writers are using variable
void reader_entry(int p) {
  lock_acquire(users_lock);
  if(nw > 0) {
    lock_acquire(serial_lock);
    Serial.print("Reader ");
    Serial.print(p);
    Serial.println("waiting for no more writers");
    if(serialEventRun) serialEventRun();
    lock_release(serial_lock);
    // if so, wait on cv
    cond_wait(users_lock, no_writers);
  }
  // increment the number of readers
  nr++;
  lock_release(users_lock);
}

void reader_exit() {
  // acquire lock for users variable
  lock_acquire(users_lock);
  // decrement number of readers
  nr--;
  // if someone is waiting for all users to exit, signal them
  if(nr == 0 && nw == 0 && cond_waiting(users_lock, no_users)) {
    cond_signal(users_lock, no_users);
  } else {
    lock_release(users_lock);
  }
}

void writer_entry(int p){
  lock_acquire(users_lock);
  // if there are any users, wait on cv
  if(nr > 0 || nw > 0) {
    lock_acquire(serial_lock);
    Serial.print("Writer ");
    Serial.print(p);
    Serial.println(" waiting for no more users");
    lock_release(serial_lock);
    cond_wait(users_lock, no_users);
  }
  // increment number of writers
  nw++;
  lock_release(users_lock);
}

void writer_exit() {
  lock_acquire(users_lock);
  // decrement number of writers
  nw--;
  if (nw == 0 && cond_waiting(users_lock, no_writers)) {
  // wake up all the readers if there are no writers
    while(cond_waiting(users_lock, no_writers)) {
      cond_signal(users_lock, no_writers); 
    }
  } else if(nr == 0 && nw == 0 && cond_waiting(users_lock, no_users)) {
    // wake up the next writer if no users
    lock_acquire(serial_lock);
    Serial.print("No users");
    lock_release(serial_lock);
    cond_signal(users_lock, no_users);
  } else {
    // no one waiting, just release the lock
//    lock_acquire(serial_lock);
//    Serial.print("no one waiting");
//    lock_release(serial_lock);
    lock_release(users_lock);
  }
}

void slow_writer() {
  while(1) {
    // enter when no other writers or readers
    writer_entry(1);
    // write to book variable
    lock_acquire(book_lock);
    lock_acquire(serial_lock);
    Serial.println("Writer 1: modifying");
    if(serialEventRun) serialEventRun();
    lock_release(serial_lock);
    book++;
    delay(250);
    lock_release(book_lock);
    // signal other waiting processes
    writer_exit();
    delay(250);
  }
}

void fast_writer() {
  while(1) {
    // enter when no other writer or readers
    writer_entry(2);
    // write to book variable
    lock_acquire(book_lock);
    lock_acquire(serial_lock);
    Serial.println("Writer 2: modifying");
    if(serialEventRun) serialEventRun();
    lock_release(serial_lock);
    book++;
    delay(100);
    lock_release(book_lock);
    // signal other waiting processes
    writer_exit();
  }
}

void fast_reader() {
  while(1) {
    // enter when no writers
    reader_entry(1);
    
    // read from the book variable
    lock_acquire(book_lock);
    lock_acquire(serial_lock);
    Serial.print("Reader 1 reading ");
    Serial.println(book);
    lock_release(serial_lock);
    lock_release(book_lock);
    
    // decrement nr and signal any waiting writers
    reader_exit();
    delay(100);
  }
}

void slow_reader() {
  while(1) {
    // enter when no writers
    reader_entry(2);
    // read from the book variable
    lock_acquire(book_lock);
    lock_acquire(serial_lock);
    Serial.print("Reader 2 reading ");
    Serial.println(book);
    lock_release(serial_lock);
    lock_release(book_lock);
    delay(300);
    // decrement nr and signal any waiting writers
    reader_exit();
    delay(250);
  }
}

// one reader and one writer
// reader reads 5 times faster than the writer, 
// then must wait for writer to finish writing before reading again
int test2_setup() {
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
  if(process_create(fast_reader, 128) < 0) {
    return 0;
  }
  if(process_create(slow_writer, 128) < 0) {
    return 0;
  }
  return 1;
}

// only one process can access at a time
// P1 writes to the variable, 
// and P2 and P3 must wait in the queue before reading
int test1_setup() {
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

// one slow writer and two readers
// readers must wait while slow writer modifies
// readers do not need to wait for each other
int test3_setup() {
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
  if(process_create(fast_reader, 128) < 0) {
    return 0;
  }
  if(process_create(slow_writer, 128) < 0) {
    return 0;
  }
  if(process_create(slow_reader, 128) < 0) {
    return 0;
  }
  return 1;
}

// two readers, two writers
// writers need to wait for everyone to clear, readers wait for writers to clear
// readers do not need to wait for each other
// writers and readers wait in turn for cv
int test4_setup() {
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
  if(process_create(fast_reader, 128) < 0) {
    return 0;
  }
  if(process_create(slow_writer, 128) < 0) {
    return 0;
  }
  if(process_create(slow_reader, 128) < 0) {
    return 0;
  }
  if(process_create(fast_writer, 128) < 0) {
    return 0;
  }
  return 1;
}

void setup() {
  // change the number below to test any of the four test cases
  if(!test1_setup()) {
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
