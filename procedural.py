
from dataclasses import dataclass, field
from typing import List
import csv
import re
import os
from glob import glob

@dataclass
class Product:
    name: str
    price: float = 0.0

@dataclass 
class ProductStock:
    product: Product
    quantity: int

@dataclass 
class Shop:
    cash: float = 0.0
    stock: List[ProductStock] = field(default_factory=list)

@dataclass
class Customer:
    name: str = ""
    budget: float = 0.0
    shopping_list: List[ProductStock] = field(default_factory=list)

def create_and_stock_shop():
    # create shop and assign values
    s = Shop()
    with open('stock.csv') as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=',')
        first_row = next(csv_reader)
        s.cash = float(first_row[0])
        for row in csv_reader:
            global p
            p = Product(row[0], float(row[1]))
            ps = ProductStock(p, float(row[2]))
            s.stock.append(ps)
    return s

global product_name

def find_product_price(product_name):
    with open('stock.csv') as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=',')
        first_row = next(csv_reader)
        for row in csv_reader:
            if product_name == row[0]:
                return row[1]
        return 0

def find_product(product_name):
    with open('stock.csv') as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=',')
        first_row = next(csv_reader)
        for row in csv_reader:
            if product_name == row[0]:
                return 1
        return 0

def find_product_qty(product_name):
    with open('stock.csv') as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=',')
        first_row = next(csv_reader)
        for row in csv_reader:
            if product_name == row[0]:
                return row[2]
        return 0

def read_customer(file_path):
    with open(file_path) as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=',')
        first_row = next(csv_reader)
        c = Customer(first_row[0], float(first_row[1]))
        for row in csv_reader:
            name = row[0]
            quantity = int(row[1])
            price = float(find_product_price(name))
            p = Product(name, price)
            ps = ProductStock(p, quantity)
            c.shopping_list.append(ps)
        return c 
        
def print_product(p):
    print(f'\nPRODUCT NAME: {p.name}\nPRODUCT PRICE: €{str(round(p.price, 2))}')  

def print_customer(c):
    print(f'CUSTOMER NAME: {c.name} \nCUSTOMER BUDGET: {c.budget}')
    total = 0.0
    out_of_stock = []
    active_product_list = []

    for item in c.shopping_list:
        # check weather this product in stock
        check_val = find_product_price(item.product.name)
        if check_val == 0:
            print('')
            print(f'ERROR Invalid line {item.product.name} in Shop stock.')
        else:
            print_product(item.product)
            print('')
            print(f'{c.name} ORDERS {item.quantity} OF ABOVE PRODUCT')
            if int(find_product_qty(item.product.name)) < int(item.quantity):
                data = {
                    "product_name" : item.product.name,
                    "availble_qty" : find_product_qty(item.product.name),
                    "need_qty": item.quantity
                }
                out_of_stock.append(data)

            cost = item.quantity * item.product.price
            buying_data = {
                "product" : item.product.name,
                "qty" : item.quantity,
                "sub_tot" : cost
            }
            active_product_list.append(buying_data)
            
            total = total + cost
            print(f'The cost to {c.name} will be €{str(round(cost, 2))}')
    print('--------------------------------------------------')
    print(f'The total cost to {c.name} will be €{total}')

    if len(out_of_stock) == 0:
        if float(c.budget) >= float(total):
            print('--------------------------------------------------')
            print('SUCCESS !')
            print(f"{c.name}'s budget is {c.budget}")
            print(f"{c.name} has enough money")
            print(f"total of {total} will be deducted from {c.name}'s budget")
            # updating the stock
            for stock in s.stock:
                # print(stock)
                for product in active_product_list:
                    if stock.product.name == product['product']:
                        # print('product name is there')
                        stock.quantity = float(stock.quantity) - float(product['qty'])
                        
            c.budget = float(c.budget) - float(total)
            s.cash = float(s.cash) + float(total)
            print(f"John's budget is {c.budget}")
            print(f"total of {total} added to shop")
            print(f"Shop has {s.cash}")
        else:
            print('--------------------------------------------------')
            print('FAIL !')
            print(f"{c.name}'s budget is {c.budget}")
            print(f'The total cost of all items is ...! {float(total)}')
            print(f"{c.name} does not have enough money")
            print(f"total of 0 will be deducted from {c.name}'s budget")
            print(f"John's budget is {c.budget}")
            print(f"total of 0 added to shop")
            print(f"Shop has {s.cash} in cash")
    else:
        for out_stock in out_of_stock:
            print(f"Not enough {out_stock['product_name']} in stock")
            print(f"{out_stock['availble_qty']} {out_stock['product_name']} in stock")
            print(f"{c.name} want {out_stock['need_qty']} {out_stock['product_name']}")
            print("Please revise order and upload again!")

        
def print_shop(s):
    print('---------------')
    print('SHOP STOCK INFO')
    print('---------------')
    print(f'CASH: {s.cash}')
    
    for item in s.stock:
        print_product(item.product)
        print(f'QUANTITY {int(item.quantity)}')

active = True  # This is our on/off switch for the program. True is on and False is off.
s = create_and_stock_shop()

while active:  # We make a while loop. So while active(True) - keep repeating everything inside
    print("---------------------------------------------------")
    print_shop(s)
    print("------------")
    print("")
    print("Select your choice:")
    print("")
    print("[1] - Enter new order") 
    PATH = ""
    EXT = "*.csv"
    all_csv_files_ = [file
                    for path, subdir, files in os.walk(PATH)
                    for file in glob(os.path.join(path, EXT))]
    
    all_csv_files = []
    for file_path in all_csv_files_:
        if file_path != "stock.csv":
            all_csv_files.append(file_path)

    index = 1
    for file_name in all_csv_files:
        index = index + 1
        print(index,')',file_name)

    print("[2] Show shop stock")
    print("[3] Exit application")
    print("[4] Read customer.csv file")
    print("[5] Read OutOfBudget.csv file")
    print("[6] Read Wants_too_much.csv file")
    print("")
    
    
    try:  # This is a try statement used to handle errors
        answer = input("Option selected: ")  
                                     
        if int(answer) == 1:

            custom_active = True
            print("")
            custom_name = input('Enter customer name : ')
            custom_budget = input('Enter budget : ')
            active_product_list = []

            while custom_active:

                custom_product = input('Enter product name : ')
                if not find_product(custom_product):
                    print('This product : {} not in stock '.format(custom_product))
                else:
                    print(custom_product)
                
                    custom_count = input('Enter quantity : ')
                    if int(find_product_qty(custom_product)) >= int(custom_count):

                        custom_sub_total = float(find_product_price(custom_product)) * int(custom_count)
                        print(f'Total cost: €{str(round(custom_sub_total, 2))}')
                        data = {
                            "product" : custom_product,
                            "qty" : custom_count,
                            "sub_tot" : custom_sub_total
                        }
                        active_product_list.append(data)

                    else:
                        print(f'Not enough {custom_product} in stock ')
                        print('ERROR: Please revise order and try again')


                close_tag = input('Would you like another product? Y/N ')
                if close_tag == 'N':
                    custom_active = False
                
            print('Summary')
            if len(active_product_list) > 0:
                total = 0
                for product in active_product_list:
                    total = total + float(product['sub_tot'])
                
                if float(total) > float(custom_budget):
                    print('FAIL! ')
                    print(f"Your budget is ...")
                    print(f'The total cost of all items is . {total}')
                    print("Customer order not valid (not enough money")
                    print(f"Total of 0 will be deducted from your budget")
                    print(f"Your budget is {custom_budget}")
                    print(f"Shop has {round(float(s.cash), 2)} in cash")

                else:
                    print("Customer order is valid (shop stock updated)")
                    print(f"Your budget is {custom_budget}")
                    print(f"Total of {round(total,2)} deducted from your budget")
                    print(f"Total of {round(total,2)} added to shop")
                    #update the stock
                    for stock in s.stock:
                        for product in active_product_list:
                            if stock.product.name == product['product']:
                                stock.quantity = float(stock.quantity) - float(product['qty'])


                    shop_total = float(s.cash) + float(total)
                    s.cash = shop_total
                    custom_budget = float(custom_budget) - float(total)
                    print(s.stock[0].product.name, s.stock[0].quantity)
                    
                    print(f"Shop has {round(shop_total, 2)} in cash")
            else:
                print('something went wrong')
                
            print("---------------------------------------------------")

        elif int(answer) == 2:
            print("---------------------------------------------------")
            print_shop(s)

        elif int(answer) == 3:  # This is how we exit the program. We make active go from True to False.
            print("Application finished!")
            break

        elif int(answer) == 4:
            with open('customer.csv') as csv_file:
                csv_reader = csv.reader(csv_file, delimiter=',')
                line_count = 0
                for row in csv_reader:
                    if line_count == 0:
                        print("----------------------------")
                        print("CUSTOMER ORDER INFO")
                        print("----------------------------")
                        print(f'NAME: {row[0]}  \nBUDGET:€{row[1]}')

                        line_count += 1
                    else:
                        print(f'PRODUCT: {row[0]} QUANTITY: {row[1]}')
                        line_count += 1
                print(f'PRODUCTS NUMBER: {line_count - 1}')
            
        elif int(answer) == 5:
            with open('OutOfBudget.csv') as csv_file:
                csv_reader = csv.reader(csv_file, delimiter=',')
                line_count = 0
                for row in csv_reader:
                    if line_count == 0:
                        print("----------------------------")
                        print("CUSTOMER ORDER INFO")
                        print("----------------------------")
                        print(f'NAME: {row[0]}  \nBUDGET:€{row[1]}')

                        line_count += 1
                    else:
                        print(f'PRODUCT: {row[0]} QUANTITY: {row[1]}')

        elif int(answer) == 6:
            with open('Wants_too_much.csv') as csv_file:
                csv_reader = csv.reader(csv_file, delimiter=',')
                line_count = 0
                for row in csv_reader:
                    if line_count == 0:
                        print("----------------------------")
                        print("CUSTOMER ORDER INFO")
                        print("----------------------------")
                        print(f'NAME: {row[0]}  \nBUDGET:€{row[1]}')

                        line_count += 1
                    else:
                        print(f'PRODUCT: {row[0]} QUANTITY: {row[1]}')
                      
                  
    except Exception as e: 
        print(e)
        print("NameError: Please Use Numbers Only")
       