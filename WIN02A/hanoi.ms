function move(n, a, b, c){
	if (n >= 1){
		move(n-1, a, c, b)
		printf("‰~”Õ#%d: ’Œ%d ---> ’Œ%d\n", n, a, c)
		cnt++
		move(n-1, b, a, c)
	}
}

var n, cnt = 0

print("N = ")
n = int(input())
move(n, 1, 2, 3)
printf("‡ŒvˆÚ“®‰ñ”%d\n", cnt)

