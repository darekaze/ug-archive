package main

import (
	"fmt"
	"sort"
)

func main() {
	arr1 := []int{0, 2020, 3, -1, -4}
	res := sum3(arr1, 2018)
	fmt.Println(res)
}

func sum3(arr []int, tgt int) string {
	sort.Ints(arr)
	fmt.Println("Ints: ", arr)

	n := len(arr) - 1

	// With repetition allowed
	for i := 0; i < n; i++ {
		start := i
		end := n

		for end > start {
			sum := arr[i] + arr[start] + arr[end]
			if sum == tgt {
				fmt.Printf("Result found: %d, %d, %d\n", arr[i], arr[start], arr[end])
				return "YES"
			}
			if sum > tgt {
				end--
			} else {
				start++
			}
		}
	}
	return "NO"
}
