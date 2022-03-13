import csv

class Product:

    def __init__(self, name, price=0):
        self.name = name
        self.price = price
    
    def __repr__(self):
        return f'NAME: {self.name} PRICE: {self.price}'


class ProductStock:
    
    def __init__(self, product, quantity):
        self.product = product
        self.quantity = quantity
    
    def name(self):
        return self.product.name 

    def unit_price(self):
        return self.product.price
        
    def cost(self):
        return self.unit_price() * self.quantity

    
        
    def __repr__(self):
        return f"{self.product} QUANTITY: {self.quantity}"


class Customer:

    def __init__(self, path):
        self.shopping_list = []


        with open(path) as csv_file:
            csv_reader = csv.reader(csv_file, delimiter=',')
            first_row = next(csv_reader)
            self.name = first_row[0]
            self.budget = float(first_row[1])
            for row in csv_reader:
                name = row[0]
                quantity = float(row[1])
                p = Product(name)
                ps = ProductStock(p, quantity)
                self.shopping_list.append(ps) 
    
   
    def calculate_costs(self, price_list):
        for shop_item in price_list:
            for list_item in self.shopping_list:
                if (list_item.name() == shop_item.name()):
                    list_item.product.price = shop_item.unit_price()
    
    def order_cost(self):
        cost = 0
        
        for list_item in self.shopping_list:
            cost += list_item.cost()
        
        return cost
    
    def __repr__(self):
        
        str = f"{self.name} wants to buy"
        for item in self.shopping_list:
            cost = item.cost()
            str += f"\n{item}"
            if (cost == 0):
                str += f" {self.name} doesn't know the price"
            else:
                str += f" COST: {cost}"
                
        str += f"\nThe cost would be: {self.order_cost()}, customer would have {self.budget - self.order_cost()} left"
        return str 


class Basket:

    def __init__(self,ProductName,ProductQuantity,CustomerBudget):
        self.shopping_list = []
        self.budget = CustomerBudget
        p = Product(ProductName)
        ps = ProductStock(p, ProductQuantity)
        self.shopping_list.append(ps)


    def calculate_costs(self, price_list):
            for shop_item in price_list:
                for list_item in self.shopping_list:
                    if (list_item.name() == shop_item.name()):
                        list_item.product.price = shop_item.unit_price()                


class Shop:
    
    
    def __init__(self, path):
        self.stock = []
        with open(path) as csv_file:
            csv_reader = csv.reader(csv_file, delimiter=',')
            first_row = next(csv_reader)
            self.cash = float(first_row[0])
            for row in csv_reader:
                p = Product(row[0], float(row[1]))
                ps = ProductStock(p, float(row[2]))
                self.stock.append(ps)

    
    def process_customer_order(self,customer_order):
        str = ""
        
        for shop_item in self.stock:
            for list_item in customer_order.shopping_list:
                if (list_item.name() == shop_item.name()):  
                    if customer_order.budget-list_item.cost() < 0:
                        str="Not enough money to process this order\n" 
                    else:
                        customer_order.budget-=list_item.cost() 
                        self.cash+=list_item.cost()
                        shop_item.quantity -= list_item.quantity
        str += f"Shop cash now is: {self.cash}"  f"\nCustomer's budget now is: {round(customer_order.budget,2)}\n"
        return str

    # method to print all names                                   
    def names(self):
        str=""
        for item in self.stock:
            str+=f"\nPRODUCT NAME: {item.name()}\n"
            str+=f"PRODUCT PRICE: €{item.unit_price()}\n"
            str+=f"QUANTITY {item.cost()}\n"
            str+=f"\n"
        return str

#check quantity in stock. 
    def product_in_stock(self,product_to_check,qty):
        for item in self.stock:
            if item.name() == product_to_check and item.quantity>=qty:
                #return True 
                #below temporary code just for testing.
                return print (f'{item.name()}' + " " + f'{str(item.quantity)})')


    def buy_product(self,name,qty):
        for item in self.stock:
            if item.name() == name and item.quantity>=qty:
                item.quantity-=qty
                return True
        

    def add_product_qty_to_stock(self,name,qty):
        for item in self.stock:
            if item.name() == name:
                item.quantity+=qty
                return True 

    def shop_cash(self,path):
        with open(path) as csv_file:
            csv_reader = csv.reader(csv_file, delimiter=',')
            first_row = next(csv_reader)
            self.cash = float(first_row[0])  
            print(f"CASH: {self.cash}")


    def out_of_budget_read_csv(self):
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

    def wants_too_much_read_csv(self):
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


    def shop_info(self):
        print('---------------')
        print('SHOP STOCK INFO')
        print('---------------')

    def menu(self):
        print("Select your choice:")
        print("")
        print("[1] - Enter new order")
        print("[2] Show shop stock")
        print("[3] Exit application")
        print("[4] Process customer.csv file")
        print("[5] Process OutOfBudget.csv file")
        print("[6] Read Wants_too_much.csv file")
        print("") 


    
    def __repr__(self):
        str = ""
        str += f'Shop has {self.cash} in cash\n'
        for item in self.stock:
            str += f"{item}\n"
        return str
