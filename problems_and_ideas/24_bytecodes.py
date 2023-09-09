from itertools import permutations

# Define the sequence of numbers
sequence = [1, 2, 3, 4]

# Generate all permutations
all_permutations = list(permutations(sequence))

# Convert each permutation to an integer representation
perm_integers = [int(''.join(map(str, perm))) for perm in all_permutations]

# Sort the permutations numerically
sorted_permutations = sorted(zip(all_permutations, perm_integers), key=lambda x: x[1])

# Extract the sorted permutations
sorted_permutations = [perm for perm, _ in sorted_permutations]

# Print the sorted permutations
for perm in sorted_permutations:
    print(perm)
