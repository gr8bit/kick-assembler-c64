#importonce 

/*
 * Returns a boolean indicating whether the value is in the list (true) or not.
 */
.function List_includes(list, value) {
	.return List_indexOf(list, value) >= 0
}

/*
 * Finds the first index of given value in list, returning the index or -1 if
 * value is not in list.
 */
.function List_indexOf(list, value) {
	.for (var i = 0; i < list.size(); i++) {
		.if (list.get(i) == value) {
			.return i		
		}
	}
	.return -1
}

/*
 * Remove duplicate elements from list in-place, keeping only the first
 * occurrence of each value.
 */
.function List_unique(list) {
	.for (var i = 0; i < list.size(); i++) {
		.var value = list.get(i)
		.for (var j = list.size() - 1; j > i; j--) {
			.if (list.get(j) == value) {
				.eval list.remove(j)
			}
		}
	}
}
