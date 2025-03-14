import numpy as np
import math 

class Arrow_Debreu:
    def __init__(self,security_type,loc_vecs,security_vec,market_states_vec,prices_vec):
        """
            Substantiate data to be used to populate the Arrow-Debreu Market Model
        """
        self.security_types = security_type
        self.market_states_vec = []
        self.security_vec = [security_vec[0:loc_vecs[0][0]]]
        self.prices_vec = [prices_vec[0:loc_vecs[0][0]]]
        self.substantiate_data(loc_vecs,security_vec,market_states_vec,prices_vec)
        
        

    def substantiate_data(self,loc_vecs,security_vec,market_states_vec,prices_vec):
        """
            Eventually spin this out into a separate "data formating" class or something

            Translates data from standard vec into vectors with different securities. 
            Allows for easy separate block pricing and Q evaluation while 
            lowering storage costs associated with sparse matrix M. 

            Inputs: 
                    loc_vec: A 2D array containing locations for the beginning of each new 
                             security's data in the security_vec and market_states_vec in it's 
                             first and second entries
                    security_vec: a 1D array containing security values
                                    if options: values = strikes
                                    if cash   : values = "1" 
                                    if ... ?? 
                    market_states_vec: a 1D array containing values of underlying market
                                    For now it's only strikes at which to price options. 
                                    I can't tell if the model can be used to price other things, 
                                        leading to other interpretations of this vector. 
                    prices_vec: a 1D array containing prices of securities 
                                    has the natural bijection to the security_vec elements
        """

        for i in range(1,len(loc_vecs)):
            # Python is weird as heck for letting us do this
            self.security_vec += [security_vec[loc_vecs[0][i-1]:loc_vecs[0][i]] ]
            self.prices_vec += [prices_vec[loc_vecs[0][i-1]:loc_vecs[0][i]] ]
        
        self.market_states_vec = [market_states_vec[0:loc_vecs[1][0]]] 
        + [market_states_vec[loc_vecs[1][i-1:loc_vecs[1][i]]] for i in range(1,len(loc_vecs[1])) ]

        return
    
    def option_pricer(self,)

    def compute_M_blocks(self):
        for sec_type in self.security_types:




