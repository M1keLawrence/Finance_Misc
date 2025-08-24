def max_sort_cost(arr):
    """
    Compute the maximum total cost under these rules:
      - You may only move 1's to the right.
      - When you move a 1, it must jump all the way to sit immediately before the next 1;
        the last 1 (if it has zeros to its right) may move to the far right end.
      - A move's cost = 1 + (distance to the next 1 at that moment).
    Returns the maximum achievable total cost for the given binary list `arr`.
    """
    n = len(arr)
    ones = [i for i, v in enumerate(arr) if v == 1]
    k = len(ones)
    if k == 0:
        return 0

    # suffix_zero[i] = number of zeros in arr[i:]
    suffix_zero = [0] * (n + 1)
    for i in range(n - 1, -1, -1):
        suffix_zero[i] = suffix_zero[i + 1] + (1 if arr[i] == 0 else 0)

    # z_j: zeros to the right of each 1_j
    z = [suffix_zero[idx + 1] for idx in ones]  # length k

    # d_j: zeros between consecutive ones (simple arithmetic)
    d = [ones[i + 1] - ones[i] - 1 for i in range(k - 1)]

    # Total distance contribution is sum(z) (INCLUDES last 1's zeros)
    distance_sum = sum(z)

    # Max number of moves M_max (each contributes a +1)
    # M_max = k * 1_{z_k > 0} + sum_{t=1..k-1} t * 1_{d_t > 0}
    moves = 0
    if z[-1] > 0:
        moves += k
    for t in range(k - 1):
        if d[t] > 0:
            moves += (t + 1)

    return distance_sum + moves

# Test: 
arr1 = [0,1,0,1,1,1,0,0,0,0,1] # 12
arr2 = [0,1,1,1,0] # 6
arr3 = [0,1,1,1,1,1,0,1,1,0,0,0,0,1] # 45

print(max_sort_cost(arr1))
print(max_sort_cost(arr2))
print(max_sort_cost(arr3))