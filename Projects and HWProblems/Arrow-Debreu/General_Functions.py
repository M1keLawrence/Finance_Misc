import numpy as np
import math

def option_payoff_mat(option_strikes, spot_prices,type = "P"):
        """
            Returns a matrix of payoff for the vector of option strikes and underlying spot values

            Inputs: 
                    option_data: A 2D array 
                                option_data[0] = "option strikes"
                                option_data[1] = "underlying spot prices" 
                    type: type of simple european option to be priced
                            Currently supports only Puts, Calls. 
                            In practice such contracts would be their own class. 
        """
        rows = len(option_strikes); cols = len(spot_prices)
        mat = np.empty([rows,cols])

        if type == "P":
            for row in range(rows):
                        for col in range(cols):
                            mat[row,col] = max(option_strikes[row] - spot_prices[col], 0)
        else: 
             for row in range(rows):
                  for col in range(cols):
                       mat[row,col] = max(spot_prices[col] - option_strikes[row], 0)
        
        return mat

def Root_Mean_SQ_Error(approximations,true_values):
    """
        Computes the Root-Mean-Squared-Error between approximations and their true values
    """
    Squared_sum = sum([math.pow((approximations[i] - true_values[i])/true_values[i],2) for i in range(len(true_values))])

    return math.pow(Squared_sum/len(true_values),0.5)

def Mean_Absolute_Error(approximations,true_values):
    """
        Computes the Absolute error between approximations and their true values
    """

    return sum([abs(approximations[i] - true_values[i])/true_values[i] for i in range(len(true_values))])/len(true_values)