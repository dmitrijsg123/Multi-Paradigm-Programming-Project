
from itertools import product
from oop import Customer, Shop
from oop import ProductStock
from oop import Basket

shop = Shop('stock.csv')
rich_customer_order=Customer('customer.csv')
rich_customer_order.calculate_costs(shop.stock)
customer_poor=Customer('OutOfBudget.csv')

shop.shop_info()
shop.shop_cash('stock.csv')
print(shop.names())

custom_active = True
while custom_active:
    shop.menu()
    answer = input("Option selected: ")


    if int(answer) == 1:
        print("1 selected")
        ProductName = input("Enter product: ")
        ProductQuantity = int(input("Enter quantity: "))
        CustomerBudget = int(input("Enter your budget: "))
        my_basket = Basket(ProductName,ProductQuantity,CustomerBudget)
        my_basket.calculate_costs(shop.stock)
        print(shop.process_customer_order(my_basket))

    elif int(answer) == 2:
        shop.shop_info()
        shop.shop_cash('stock.csv')
        print(shop.names())
        

    elif int(answer) == 3:

        print("Option selected: 3")
        print("Application finished!")
        custom_active = False

    elif int(answer) == 4: 
        print(shop.process_customer_order(rich_customer_order))
        


    elif int(answer) == 5:
        customer_poor.calculate_costs(shop.stock)
        print(shop.process_customer_order(customer_poor))
        

    elif int(answer) == 6:
        shop.wants_too_much_read_csv()


