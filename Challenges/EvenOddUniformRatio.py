import numpy as np
import matplotlib.pyplot as plt
from scipy.integrate import cumulative_trapezoid

# Define PDF of Z = X / Y
def f_z(z):
    return np.where(z < 1, 0.5, 0.5 / z**2)

# Define range of z values
z_vals = np.linspace(0.01, 80, 1000)
pdf_vals = f_z(z_vals)

# Compute CDF by numerical integration
cdf_vals = cumulative_trapezoid(pdf_vals, z_vals, initial=0)

# Plot
plt.plot(z_vals, cdf_vals, label="CDF of Z = X/Y")
plt.xlabel("z")
plt.ylabel("F_Z(z)")
plt.title("CDF of Z = X / Y (X, Y ~ Uniform(0,1))")
plt.grid(True)
plt.legend()
plt.show()

arr = np.array([0.5*i for i in range(1000001)])
arr[0] = 0.01
evenOdd_pdf_vals = f_z(arr)

# Compute CDF by numerical integration
evenOdd_cdf_vals = cumulative_trapezoid(evenOdd_pdf_vals, arr, initial=0)
even_prob, odd_prob = 0,0
evenOddCnt = 0
for begin, end in zip(evenOdd_cdf_vals[:-1],np.roll(evenOdd_cdf_vals,-1)):
    if evenOddCnt % 2 == 0:
        even_prob += (end - begin)
    else: 
        odd_prob += (end-begin)
    evenOddCnt += 1

print( "Using Numerical Integration.\n"
      + f"Odds z is even: {even_prob}\n"
      + f"Odds z is odd: {odd_prob}\n"
      + f"Total Odds: {even_prob + odd_prob}\n")

# Alternatively, just use the closed form cdf
def F_z(z):
    return np.where(z < 1, 0.5*z,1 - 0.5/z)

evenOdd_cdf_vals = F_z(arr)
even_prob, odd_prob = 0,0
evenOddCnt = 0
for begin, end in zip(evenOdd_cdf_vals[:-1],np.roll(evenOdd_cdf_vals,-1)):
    if evenOddCnt % 2 == 0:
        even_prob += (end - begin)
    else: 
        odd_prob += (end-begin)
    evenOddCnt += 1

print("Using the closed form CDF.\n"
      + f"Odds z is even: {even_prob}\n"
      + f"Odds z is odd: {odd_prob}\n"
      + f"Total Odds: {even_prob + odd_prob}")