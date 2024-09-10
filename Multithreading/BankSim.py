# Youtube Link: 
# Sources: https://docs.python.org/3/library/threading.html#thread-local-data
#          https://docs.python.org/3/reference/compound_stmts.html#with

import threading

class BankAccount():
    def __init__(self, initial_balance = 0):
        self.balance = initial_balance
        self.lock = threading.Lock()

    def deposit(self, amount):
        with self.lock:
            self.balance += amount
            print(f"Deposited: {amount}, New Balance: {self.balance}")
    
    def withdraw(self, amount):
        with self.lock:
            if  amount <= self.balance:
                self.balance -= amount
                print(f"Withdrew: {amount}, New Balance: {self.balance}")
            else:
                print(f"Not enough funds. Balance: {self.balance}")

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

my_account = BankAccount(50)

dt_1 = DepositThread(my_account, 20)
wt_1 = WithdrawThread(my_account, 10)
dt_2 = DepositThread(my_account, 20)
wt_2 = WithdrawThread(my_account, 50)
wt_3 = WithdrawThread(my_account, 50)

dt_1.start()
wt_1.start()
dt_2.start()
wt_2.start()
wt_3.start()

dt_1.join()
wt_1.join()
dt_2.join()
wt_2.join()
wt_3.join()

print(f"Final Balance: {my_account.balance}")