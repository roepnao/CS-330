# Youtube Link: 
# Sources: https://docs.python.org/3/library/threading.html#thread-local-data
#          https://docs.python.org/3/reference/compound_stmts.html#with
#          https://www.geeksforgeeks.org/python-using-2d-arrays-lists-the-right-way/
#          https://www.w3schools.com/python/ref_func_range.asp
#          https://www.w3schools.com/python/module_random.asp
#          https://www.linode.com/docs/guides/python-priority-queue/

import threading
from queue import PriorityQueue
from random import uniform
from time import sleep

class BankAccount():
    def __init__(self, name, initial_balance = 0):
        self.name = "Account " + str(name)
        self.balance = initial_balance
        self.priority_queue = PriorityQueue()
        self.queue_lock = threading.Lock()
        self.balance_lock = threading.Lock()

    def deposit(self, amount):
        with self.balance_lock:
            self.balance = round(self.balance + amount, 2)
            print(f"{self.name}: Deposited:{amount: .2f}, Old Balance:{self.balance - amount: .2f}, New Balance:{self.balance: .2f}")
    
    def withdraw(self, amount):
        with self.balance_lock:
            if  amount <= self.balance:
                self.balance = round(self.balance - amount, 2)
                print(f"{self.name}: Withdrew:{amount: .2f}, Old Balance:{self.balance + amount: .2f}, New Balance:{self.balance: .2f}")
            else:
                print(f"{self.name}: Not enough funds to withdraw{amount: .2f}, Balance:{self.balance: .2f}")

    def update(self):
        with self.queue_lock:
            if self.priority_queue.empty():
                return
            priority, amount = self.priority_queue.get()

        if priority == 0:
            self.deposit(amount)
        elif priority == 1:
            self.withdraw(amount)

class DepositThread(threading.Thread):
    def __init__(self, account, amount):
        super().__init__()
        self.account = account
        self.amount = amount

    def queue(self):
         with self.account.queue_lock:
                self.account.priority_queue.put((0, self.amount))

    def run(self):
        self.queue()
        sleep(0.1)
        self.account.update()

class WithdrawThread(threading.Thread):
    def __init__(self, account, amount):
        super().__init__()
        self.account = account
        self.amount = amount

    def queue(self):
         with self.account.queue_lock:
                self.account.priority_queue.put((1, self.amount))

    def run(self):
        self.queue()
        sleep(0.1)
        self.account.update()

TOTAL_ACCOUNTS = 25
THREAD_COUNT = 50

# Create account array
accounts = list()
starting_balance = range(100, ((TOTAL_ACCOUNTS * 100) + 100), 100)
for i in range(TOTAL_ACCOUNTS):
    accounts.append(BankAccount(i+1, starting_balance[i]))

# 2d array of deposit threads
deposit_array = [[0 for i in range(THREAD_COUNT)] for j in range(TOTAL_ACCOUNTS)]
for i in range(TOTAL_ACCOUNTS):
     for x in range(THREAD_COUNT):
        deposit_array[i][x] = (DepositThread(accounts[i], round(uniform(0.01, (TOTAL_ACCOUNTS * 100)), 2)))
# 2d array of withdraw threads
withdraw_array = [[0 for i in range(THREAD_COUNT)] for j in range(TOTAL_ACCOUNTS)]
for i in range(TOTAL_ACCOUNTS):
     for x in range(THREAD_COUNT):
        withdraw_array[i][x] = (WithdrawThread(accounts[i], round(uniform(0.01, TOTAL_ACCOUNTS * 100), 2)))

# Start Threads
for i in range(TOTAL_ACCOUNTS):
    for x in range(THREAD_COUNT):
        deposit_array[i][x].start()
        withdraw_array[i][x].start()
# Join threads
for i in range(TOTAL_ACCOUNTS):
    for x in range(THREAD_COUNT):
        deposit_array[i][x].join()
        withdraw_array[i][x].join()

# prints final balance of all acounts
print()
for i in range(TOTAL_ACCOUNTS):
    print(f"{accounts[i].name}: Final Balance:{accounts[i].balance: .2f}")