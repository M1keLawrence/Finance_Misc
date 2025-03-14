import functools
import math

# Nonlinear Equation solvers

def NLin_Bisection(f,a,b, int_tol, int_approx):
    """
    Input: 
            a,b: start and end of initial interval
            f: function to be solved
            tol_int: largest admissible size of active interval when solution is found
            tol_approx: largest admissible value of |f(x)| when solution is found
    Output: 
            x_M: approximate solution for f(x) = 0
    """
    iter_count = 0
    xL = a; xR = b 
    print("Bisection Method Output:\n")
    while(max(abs(f(xL)), abs(f(xR))) > int_approx or (xR - xL) > int_tol):

        xM = (xR+xL)/2
        print("Current guess: ", xM, " :: f(xM): ", f(xM), " :: Iteration:", iter_count,"\n")

        if f(xM) == 0:
            return xM

        if(f(xL)*f(xM) < 0):
            xR = xM
        else: 
            xL= xM

        iter_count += 1

    return xM

def NLin_Newtons_SingleVar(f,f_prime,x0,tol_consec,tol_approx):
    """
    Input: 
            x0: initial guess
            f: given function
            f_prime: derivative of given function
            tol_consec: largest admissible distance between two consecutive approximations when solution is found
            tol_approx: largest admissible value of |f(x)| when solution is found
    Output: 
            x_new: approximate solution for f(x) = 0
    """
    iter_count = 0
    x_new = x0; x_old = x0 - max(tol_approx,tol_consec) - 0.001

    print("Newton's method output:\n")
    while(abs(f(x_new) - f(x_old)) > tol_consec or abs(f(x_new)) > tol_approx):

        if f(x_new) == 0: 
            return x_new

        print("Current guess: ", x_new," :: f(x_new): ",f(x_new)," :: Iteration: ", iter_count,"\n") 
        x_old = x_new

        x_new = x_old - f(x_old)/f_prime(x_old)
        iter_count += 1
    return x_new

def NLin_Secant(f,x0,x1,tol_consec,tol_approx):
    """
    Input: 
            x0: first guess
            x1: second guess
            f: given function
            tol_consec: largest admissible largest admissible distance between two consecutive approximations when solution is found
            tol_approx: largest admissible value of |f(x)| when solution is found
    Output: 
            x_new: approximate solution for f(x) = 0
    """
    iter_count = 0
    x_new = x1; x_old = x0

    print("Secant method output:\n")
    while(abs(f(x0)) > tol_approx or abs(x_new - x_old) > tol_consec):
        print("Current guess: ", x_new," :: f(x_new): ",f(x_new)," :: Iteration: ", iter_count,"\n")

        if f(x_new) == 0:
            return x_new

        x_oldest = x_old
        x_old = x_new

        x_new = x_old - f(x_old)*(x_old-x_oldest)/ (f(x_old) - f(x_oldest))
        iter_count += 1
    return x_new

def calc_coupons(annual_rate,coupon_rate, n):
    """
    Purpose: 
            To create a vector of the coupon payments of a bond. 
    Input: 
            c_rate: the coupon rate for face value of 100
            n: the number of coupon payments
    Output: 
            coupons: a list of the bond's coupon payments 
    """
    coupons = [annual_rate*coupon_rate*100]*(n-1)
    coupons.append(100*(1 + annual_rate*coupon_rate))
    return coupons

def calc_cash_flow_times(annual_rate, end):
    """
    Purpose: 
            To create a vector of the cash flow times for a bond. 
    Input: 
            annual_rate: the rate at which cash flows occur
            end: the remaining age of the bond
    Output: 
            t_cash_flows: a list of the times of future cash flows, 1 == a year in the future
    """
    if annual_rate < 1:
        t_cash_flows = [end - annual_rate*i for i in range(0,math.ceil(end/annual_rate)) if (end - annual_rate*i) > 0]
        return t_cash_flows[::-1]
    if annual_rate >=1:
         t_cash_flows = [end - annual_rate*i for i in range(0,math.ceil(end*annual_rate)) if (end - annual_rate*i) > 0]
         return t_cash_flows



def Cum_Dist_StdNormal(t):
    """
        Implements Abramowitz and Stgun's approximation of the standard normal distribution (Handbook of Mathematical Functions pg 932).
        
        Inputs: 
                t: the variable to be integrated through
        Outputs: 
                The result of the approximation

        To be fixed: 
                    Implement Horner's algo
    """
    z = math.fabs(t)
    y = 1/(1 + 0.2316419*z)
    m = 1 - math.exp(-(t*t)/2)*(0.319381530*y - 0.356563782*y*y + 1.781477937*y*y*y - 1.821255978*y*y*y*y + 1.330274429*y*y*y*y*y)/math.sqrt(2*math.pi)
    
    if t > 0: 
        return m
    else: 
        return (1 - m)

