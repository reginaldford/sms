#replaces the first line of every .c, .h, .y , .l
for file in *.c *.h *.y *.l; do sed -i '1s/.*/\/\/\ Read\ https:\/\/raw.githubusercontent.com\/reginaldford\/sms\/main\/LICENSE.txt\ for\ license\ information/' "$file"; done
