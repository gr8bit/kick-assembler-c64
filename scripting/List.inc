#importonce 

// make a shallow copy of the given list
.function List_copy(list) {
	.return List().addAll(list)
}

// count all occurrences of value in the list, returning the number of occurrences.
.function List_count(list, value) {
	.var result = 0
	.for (var i = 0; i < list.size(); i++) {
		.if (list.get(i) == value) {
			.eval result += 1
		}
	}
	.return result
}

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
 * Finds all indexes of given value in list, returning a List of indexes
 * which is empty if value is not in the list.
 */
.function List_indexesOf(list, value) {
	.const result = List()
	.for (var i = 0; i < list.size(); i++) {
		.if (list.get(i) == value) {
			.eval result.add(i)		
		}
	}
	.return result
}

/*
 * Splits the given List into smaller lists of `size` entries.
 *
 * Example:
 * .const list = List().add(1,2,3,4,5,3,6,3)
 * .var split = List_splitSize(list, 3) => ( (1,2,3), (4,5,3), (6,3) )
 */
.function List_splitSize(list, size) {
	.return List_splitSize(list, size, null)
}

/*
 * Splits the given List into smaller lists of `size` entries. Number of resulting lists can be
 * limited by supplying the `limit` number, which will put all the remaining entries into the
 * last list item returned.
 *
 * Example:
 * .const list = List().add(1,2,3,4,5,3,6,3)
 * .var split = List_splitSize(list, 3, 1) => ( (1,2,3), (4,5,3,6,3) )
 * .var split = List_splitSize(list, 3, 2) => ( (1,2,3), (4,5,3), (6,3) )
 * .var split = List_splitSize(list, 3, null) => ( (1,2,3), (4,5,3), (6,3) )
 */
.function List_splitSize(list, size, limit) {
	.errorif limit && limit < 0, "limit must be null or >= 0 but is: "+limit	
	.if (limit == 0) {
		// split limit is zero, so just return a list copy
		.return List_copy(list)
	}
	.var result = List()
	.var part = List()
	.for (var i = 0; i < list.size(); i++) {
		// i > 0 is needed to prevent the empty list from being added right away
		.if (i > 0 && mod(i, size) == 0 && (limit == null || result.size() < limit)) {
			.eval result.add(part)
			.eval part = List()
		}
		.eval part.add(list.get(i))
	}
	.if (part.size() > 0) {
		.eval result.add(part)
	}
	.return result
}

/*
 * Splits a list at given value into smaller lists, returning a List of Lists. The split value
 * is not included in the resulting lists.
 *
 * Example:
 * .const list = List().add(1,2,3,4,5,3,6,3)
 * .var split = List_splitValue(list, 3) => ( (1,2), (4,5), (6), () )
 */
.function List_splitValue(list, atValue) {
	.return List_splitValue(list, atValue, null)
}

/*
 * Splits a list at given value into smaller lists, returning a List of Lists. The split value
 * is not included in the resulting lists.
 * The number of splits can be limited - the last list will then contain the rest of the original
 * List. Pass limit of null to not limit the number of splits.
 *
 * Examples:
 * .const list = List().add(1,2,3,4,5,3,6,3)
 * .var split = List_splitValue(list, 3, 1) => ( (1,2), (4,5,3,6) )
 * .var split = List_splitValue(list, 3, 2) => ( (1,2), (4,5), (6,3) )
 * .var split = List_splitValue(list, 3, 3) => ( (1,2), (4,5), (6) )
 * .var split = List_splitValue(list, 3, 4) => ( (1,2), (4,5), (6), () )
 * .var split = List_splitValue(list, 3, null) => ( (1,2), (4,5), (6), () )
 */
.function List_splitValue(list, atValue, limit) {
	.errorif limit && limit < 0, "limit must be null or >= 0 but is: "+limit	
	.if (limit == 0) {
		// split limit is zero, so just return a list copy
		.return List_copy(list)
	}
	.var result = List()
	.var part = List()
	.for (var i = 0; i < list.size(); i++) {
		.var value = list.get(i)
		.if (value == atValue && (limit == null || result.size() < limit)) {
			.eval result.add(part)
			.eval part = List()
			// special case handling: if the last value of given list is a
			// split character, also add the empty list (if limit allows it)
			.if (i == list.size() - 1 && (limit == null || result.size() < limit)) {
				.eval result.add(part)
			}
		} else {
			.eval part.add(value)
		}
	}
	.if (part.size() > 0) {
		.eval result.add(part)
	}
	.return result
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
