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

class BankAccount():
    def __init__(self, name, initial_balance = 0):
        self.balance = initial_balance
        self.priority_queue = PriorityQueue()
        self.queue_lock = threading.Lock()
        self.lock = threading.Lock()
        self.name = "Account " + str(name)

    def deposit(self, amount):
        with self.lock:
            self.balance = self.balance + amount
            print(f"{self.name}: Deposited:{amount: .2f}, Old Balance:{self.balance - amount: .2f}, New Balance:{self.balance: .2f}")
    
    def withdraw(self, amount):
        with self.lock:
            if  amount <= self.balance:
                self.balance = round(self.balance - amount, 2)
                print(f"{self.name}: Withdrew:{amount: .2f}, Old Balance:{self.balance + amount: .2f}, New Balance:{self.balance: .2f}")
            else:
                print(f"{self.name}: Not enough funds to withdraw{amount: .2f}, Balance:{self.balance: .2f}")

    def run(self):
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
            self.account.priority_queue.put((0, amount))

    def run(self):
        self.account.run()

class WithdrawThread(threading.Thread):
    def __init__(self, account, amount):
            super().__init__()
            self.account = account
            self.account.priority_queue.put((1, amount))

    def run(self):
        self.account.run()

# Create account array
accounts = list()
starting_balance = range(100, 600, 100)
for i in range(5):
    accounts.append(BankAccount(i+1, starting_balance[i]))

# 2d array of deposit threads
deposit_array = [[0 for i in range(5)] for j in range(5)]
for i in range(5):
     for x in range(5):
        deposit_array[i][x] = (DepositThread(accounts[i], round(uniform(0.01, 500), 2)))
# 2d array of withdraw threads
withdraw_array = [[0 for i in range(5)] for j in range(5)]
for i in range(5):
     for x in range(5):
        withdraw_array[i][x] = (WithdrawThread(accounts[i], round(uniform(0.01, 500), 2)))

# Start Threads
for i in range(5):
    for x in range(5):
        deposit_array[i][x].start()
        withdraw_array[i][x].start()
# Join threads
for i in range(5):
    for x in range(5):
        deposit_array[i][x].join()
        withdraw_array[i][x].join()

# prints final balance of all acounts
print('\n')
for account in accounts:
    print(f"{account.name}: Final balance:{account.balance: .2f}")