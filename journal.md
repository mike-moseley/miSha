Found nasty bug in insertToHashmap in c-data-structures/hash_map.c while implementing environment hashmap. 
```

	if (hashmap->len * 10 > hashmap->cap * 7) {
		resizeHashMap(hashmap);
	}
```
was
```
	if (hashmap->len * 10 > hashmap->len * 7) {
		resizeHashMap(hashmap);
	}

```
so every insert would grow the map since the condition was always true.
Plus some other bugs in hash_map.c; string case was never finished in multiple spots,
testing would have found this if I had bothered.
Lots of headaches and back and forth with claude to fix.
