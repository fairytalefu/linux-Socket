import time
def sayhello(str):
    print("Hello ",str)
    time.sleep(2)
name_list =['xiaozi','aa','bb','cc']
start_time = time.time()
for i in range(len(name_list)):
    sayhello(name_list[i])
print ('%d second'% (time.time()-start_time))
