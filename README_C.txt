C PROGRAM EXPLANATION

---------------
USAGE
---------------
A file stock.csv must be present always. It is read at startup to create the shop.

It has 2 modes, input customer file (to open 3 CSV files with customer's orders) and life mode.


------------------------
CUSTOMER FILE MODE
-------------------------

> shop.exe -f customer.csv
Loads up customer order that is successfully executed, shop cash updated accordingly.

shop.exe -f Wants_too_much.csv
Loads up customer order, the order cannot be executed, error message comes up.

Shop.exe -f OutOfBudget.csv
Loads up customer order, the order cannot be executed, error message comes up.

------------------------
LIFE MODE
-------------------------

> shop.exe

It loads stock.csv file, prints its content on screen and displays the following menu:

........................
Select your choice:

[1]-Enter new order
[2]-Show shop stock
[3]-Exit application


Option selected:
........................


We can here select to enter a new order, show shop stock or finish.

If we select [1], the following menu is displayed:

Select your choice:

[1]-Enter customer name [current: ]
[2]-Enter budget [current: 0.00]
[3]-Enter product name [current: ]
[4]-Enter quantity [current: 0]
[5]-Show current order info
[6]-Order products
[7]-Clean shopping list
[p]-Previous menu

Option selected:


Here we must enter the name [1], budget [2], next product to add to cart [3] and quantity [4], we can also show our current shopping list [5] or clean it [6]

Once we are ready we can do the purchase [7]

We can also return to previous menu [p]

IMPORTANT: The name, budget, product name and quantity must be entered in order (codes force this order)

We can edit name and budget at any time

If there is any error, we can clean shopping list and start again

Shop content is updated accordingly when the purchase is valid and accepted.






