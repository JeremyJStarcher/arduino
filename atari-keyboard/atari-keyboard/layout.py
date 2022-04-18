#
# import os
# os.chdir('.....')
def run(script):
    exec(open(script).read())




from pcbnew import *
board = GetBoard()
nets = board.GetNetsByName()

for netname, net in nets.items():
    print(f"{netname} code: ${net}")


Refresh()
