from datetime import date
from unicodedata import numeric
from FinancialContract import FinancialContract   
from GeneralFunctions import NLin_Newtons_SingleVar
import math
import functools

class Bond(FinancialContract):
    
    #Initialize the class
    def __init__(self, time_to_expiry, price_market, coupon_timing, coupon_rate, date_formed = date.today, y = "not computed"):
        FinancialContract.__init__(self, date_formed, time_to_expiry)  
        self.set_t_cash_flows(coupon_timing, super(Bond,self).get_time_to_expiry()) 
        self.set_v_cash_flows(coupon_rate, coupon_timing, len(self.get_t_cash_flows())) 
        self.set_price_market(price_market)
        self.set_coupon_timing(coupon_timing)

    #Get and set internal data
    def set_v_cash_flows(self, coupon_rate, annual_rate, n):
        """
        Purpose: 
                To create a vector of the coupon payments of a bond. 
        Input: 
                coupon_rate: the coupon value in percent for face value of 100
                n: the number of coupon payments
        Output: 
                coupons: a list of the bond's coupon payments 
        """
        coupons = [annual_rate*coupon_rate*100]*(n-1)
        coupons.append(100*(1 + annual_rate*coupon_rate))
        self.__v_cash_flows = coupons

    def get_v_cash_flows(self):
        return self.__v_cash_flows

    def set_t_cash_flows(self, coupon_timing, time_to_expiry):
        """
        Purpose: 
                To create a vector of the cash flow times for a bond. 
        Input: 
                coupon_timing: the rate at which cash flows occur, standardized to 1 = a year
                time_to_expiry: the remaining age of the bond
        Output: 
                t_cash_flows: a list of the times of future cash flows, 1 == a year in the future
        """
        if coupon_timing > time_to_expiry:
            t_cash_flows = [time_to_expiry]
            return
        if coupon_timing < 1:
            self.__t_cash_flows = [time_to_expiry - coupon_timing*i for i in range(0, math.ceil(time_to_expiry/coupon_timing)) if (time_to_expiry - coupon_timing*i) > 0]
            self.__t_cash_flows = self.__t_cash_flows[::-1]
            return
        if coupon_timing >=1:
             self.__t_cash_flows = [time_to_expiry - coupon_timing*i for i in range(0,math.ceil(time_to_expiry*coupon_timing)) if (time_to_expiry - coupon_timing*i) > 0]
             self.__t_cash_flows = self.__t_cash_flows[::-1]
             return

    def get_t_cash_flows(self):
        return self.__t_cash_flows

    def set_yield(self, new_y):
        self.__yield = new_y

    def get_yield(self):
        return self.__yield

    def set_price_market(self, B):
        self.__price_market = B

    def get_price_market(self):
        return self.__price_market

    def set_coupon_rate(self, coupon_rate):
        self.__coupon_rate = coupon_rate

    def get_coupon_rate(self, coupon_rate):
        return self.__coupon_rate

    def set_coupon_timing(self, coupon_timing):
        self.__coupon_timing = coupon_timing 

    def get_coupon_timing(self):
        return self.__coupon_timing 


    v_cash_flows = property(get_v_cash_flows, set_v_cash_flows)
    t_cash_flows = property(get_t_cash_flows, set_t_cash_flows)
    y = property(get_yield, set_yield)
    price_market = property(get_price_market, set_price_market)
    coupon_rate = property(get_coupon_rate, set_coupon_rate)

    #Override abc method
    def Print(self):
        print(super(Bond,self).get_Date_formed())
        print("Coupons: ", c_iter, "\n", "Times: ", t_iter, "\n", "Yield: ", y, "\n")

################################################################################
# Functions for computing a Bond's yield #######################################
################################################################################
    """
        Most (All?) of these need to be moved into a separate 'bond mathematics funcitons' class. Suck. 
        Derive it from a base 'Financial mathematics functions' class. 

        Or... @functools.cached_property !!
    """

    def calc_price_by_yield(self, y, v_cash_flows, t_cash_flows, market_price = 0.0):
        """
        Purpose: 
                Calculates price of bond. 
                    I have left v_cash_flow, t_cash_flow, and y as inputs so that I may use functools.partial 
                    to reduce to a function of single variable y for use in Newton's Method. 
                    Then they are constants embedded in the function and are not called for each computation. 
        Input: 
                y: yield of bond
                t_cash_flow: vector of cash flow dates
                v_cash_flow: vector of cash flows
        Output: 
                B: price of bond
        """
        B = 0
        for i in range(0,len(v_cash_flows)):
            B += v_cash_flows[i]*math.exp(-y*t_cash_flows[i])

        return B - market_price

    def dB_dy(self,y, v_cash_flows, t_cash_flows): 
        """
        Purpose: 
                Calculates derivative of bond wrt the yield, y
                    I have left v_cash_flow, t_cash_flow, and y as inputs so that I may use functools.partial 
                    to reduce to a function of single variable y for use in Newton's Method. 

        Input: 
                y: yield of bond
                t_cash_flow: vector of cash flow dates
                v_cash_flow: vector of cash flows
        Output:
                dB_dy: derivative of bond wrt the yield, y
        """
        dB_dy = 0
        for i in range(0,len(v_cash_flows)):
            dB_dy -= v_cash_flows[i]*t_cash_flows[i]*math.exp(-y*t_cash_flows[i])
        return dB_dy

    def d2B_dy2(self,y,v_cash_flows,t_cash_flows):
        """
        Purpose: 
                Calculates second derivative of bond wrt the yield, y
                    I have left v_cash_flow, t_cash_flow, and y as inputs so that I may use functools.partial 
                    to reduce to a function of single variable. 

        Input: 
                y: yield of bond
                t_cash_flow: vector of cash flow dates
                v_cash_flow: vector of cash flows
        Output:
                d2B_dy2: derivative of bond wrt the yield, y 
        """
        d2B_dy2 = 0
        for i in range(0,len(v_cash_flows)):
            d2B_dy2 += v_cash_flows[i]*t_cash_flows[i]*t_cash_flows[i]*math.exp(-y*t_cash_flows[i])
        return d2B_dy2

    def calc_Yield(self, x0,tol):
        """
        Inputs: 
                market_price: the market price of the bond
                t_cash_flow: vector of cash flow dates
                v_cash_flow: vector of cash flows
                tol = tolerance for declaring convergence of Newtons Method
        Outputs: 
                x_new: the yeild of the bond
        """
        B_y = functools.partial(self.calc_price_by_yield, t_cash_flows = self.__t_cash_flows, v_cash_flows = self.__v_cash_flows, market_price = self.__price_market)
        dB_dy = functools.partial(self.dB_dy, t_cash_flows = self.__t_cash_flows, v_cash_flows = self.__v_cash_flows)
        print(B_y(0))
        print(dB_dy(0))

        self.__yield = NLin_Newtons_SingleVar(B_y, dB_dy,x0,tol,tol)
        return self.__yield

    def calc_Duration(self, y = "N/A"):
        if (y != "N/A"):
            print("one")
            return -self.dB_dy(y, self.__v_cash_flows, self.__t_cash_flows) / self.__price_market
        if (self.__yield != "not computed"):
            return -self.dB_dy(self.__yield, self.__v_cash_flows, self.__t_cash_flows) / self.__price_market
        else:
            raise ValueError("Bond yield not available. ")

    def calc_Convexity(self, y = "N/A"):
        if (y != "N/A"):
            print(y)
            return self.d2B_dy2(y, self.__v_cash_flows, self.__t_cash_flows) / self.__price_market
        if (self.__yield != "not computed"):
            return self.d2B_dy2(self.__yield, self.__v_cash_flows, self.__t_cash_flows) / self.__price_market
        else:
            raise ValueError("Bond yield not available. ")

