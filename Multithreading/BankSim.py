# Youtube Link: 
# Sources: https://docs.python.org/3/library/threading.html#thread-local-data
#          https://docs.python.org/3/reference/compound_stmts.html#with
#          https://www.geeksforgeeks.org/python-using-2d-arrays-lists-the-right-way/
#          https://www.w3schools.com/python/module_random.asp

import threading
from random import uniform

class BankAccount():
    def __init__(self, name, initial_balance = 0):
        self.balance = initial_balance
        self.lock = threading.Lock()
        self.name = "Account " + str(name)

    def deposit(self, amount):
        with self.lock:
            self.balance = round(self.balance + amount, 2)
            print(f"{self.name}: Deposited:{amount: .2f}, Old Balance:{self.balance - amount: .2f}, New Balance:{self.balance: .2f}")
    
    def withdraw(self, amount):
        with self.lock:
            if  amount <= self.balance:
                self.balance = round(self.balance - amount, 2)
                print(f"{self.name}: Withdrew:{amount: .2f}, Old Balance:{self.balance + amount: .2f}, New Balance:{self.balance: .2f}")
            else:
                print(f"{self.name}: Not enough funds to withdraw{amount: .2f}, Balance:{self.balance: .2f}")

class DepositThread(threading.Thread):
    def __init__(self, account, amount):
            super().__init__()
            self.account = account
            self.amount = amount

    def run(self):
        self.account.deposit(self.amount)

class WithdrawThread(threading.Thread):
    def __init__(self, account, amount):
            super().__init__()
            self.account = account
            self.amount = amount

    def run(self):
        self.account.withdraw(self.amount)

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
for i in range(5):
    print(f"{accounts[i].name}: Final balance:{accounts[i].balance: .2f}")