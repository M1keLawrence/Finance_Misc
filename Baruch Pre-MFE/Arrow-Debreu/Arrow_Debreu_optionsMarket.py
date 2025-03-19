import numpy as np
import scipy.linalg as LA
import math 
import functools

from General_Functions import option_payoff_mat, Root_Mean_SQ_Error, Mean_Absolute_Error

class Arrow_Debreu_options_model:
    def __init__(self,put_data,call_data,market_states):
        """
            Substantiate data to be used to populate the Arrow-Debreu Market Model
            Input:
                    put_data: 2D array
                            [0], gives strikes
                            [1], gives market prices

                    call_data:2D array
                            [0], gives strikes
                            [1], gives market prices 

                    market_states: 2D array
                            Gives underlying spot prices for option payoff evaluation
                                [0], gives values s.t. put payoff is positive
                                [1], gives values s.t. call payoff is positive

                
            Notes:
                    If needed, expand to other securites (stock spot, cash) using **kwargs

                    
        """
        self.put_data = put_data
        self.call_data = call_data
        self.market_states = market_states

    def Get_Put_Data(self):
        return self.put_data
    
    def Set_Put_Data(self,new_data):
        self.put_data = new_data

    def Get_Call_Data(self):
        return self.call_data
    
    def Set_Call_Data(self,new_data):
        self.call_data = new_data

    def Get_Market_States(self):
        return self.market_states
    
    def Set_Market_States(self,new_states):
        self.market_states = new_states

    @functools.cached_property
    def compute_Payoff_blocks(self):
        """
            Creates two block payoff matrices for the call and put options
        """
        Block_1 = option_payoff_mat(self.put_data[0],self.market_states[0], type = "P")
        Block_2 = option_payoff_mat(self.call_data[0],self.market_states[1], type = "C")

        return [Block_1,Block_2]
    
    def Get_M_blocks(self):
        """
            Wrapper for intuitive retrival of the non-zero blocks in the payoff matrix
        """
        return self.compute_Payoff_blocks
    
    def Get_M_matrix(self):
        """
            Returns the payoff matrix in block-diagonal matrix form
        """
        return LA.block_diag(self.compute_Payoff_blocks[0],self.compute_Payoff_blocks[1])
    
    @functools.cached_property
    def compute_Q_vec(self):
        """
            Computes the state prices for the given class member data
        """
        Q_vec = []
        
        # compute put contribution
        LU,Piv = LA.lu_factor(self.compute_Payoff_blocks[0])
        Q_vec.extend(LA.lu_solve((LU,Piv),self.put_data[1]))
    
        # compute call contribution
        LU,Piv = LA.lu_factor(self.compute_Payoff_blocks[1])
        Q_vec.extend(LA.lu_solve((LU,Piv),self.call_data[1]))

        return Q_vec
    
    def Get_Q(self):
        """
            Wrapper for intuitive retriving of Q_vec, the vector of state prices
        """
        return self.compute_Q_vec
    
    def check_Arbitrage(self):
        if np.all(np.array(self.compute_Q_vec) >= 0):
            print("No arbitrage present")
        else: 
            print("Arbitrage present")
        return
    
    def check_completeness(self):
        if ( np.linalg.matrix_rank(self.Get_M_matrix()) == (len(self.market_states[0]) + len(self.market_states[1])) ):
            print("Market is Complete")
        else:
            print("Market is incomplete")

    def Approximate_Prices(self,put_strikes,call_strikes):

        total_states = self.market_states[0] + self.market_states[1]

        call_payoffs_mat = option_payoff_mat(call_strikes,total_states,type="C")
        put_payoffs_mat = option_payoff_mat(put_strikes,total_states,type="P")

        call_approximations = np.dot(call_payoffs_mat,self.Get_Q())
        put_approximations = np.dot(put_payoffs_mat,self.Get_Q())

        return np.append(put_approximations,call_approximations)

    def Model_RMSE(self,put_strikes,call_strikes,true_values):
        """
            Compute the Root-Mean-Squared-Error of the model's approximation to the market's midpoint prices
        """
        
        approximations = self.Approximate_Prices(put_strikes,call_strikes)

        return Root_Mean_SQ_Error(approximations,true_values)
        

        
    
    def Model_MAE(self,put_strikes,call_strikes,true_values):
        """
            Compute the Mean-Absolute--Error of the model's approximation to the market's midpoint prices
        """

        approximations = self.Approximate_Prices(put_strikes,call_strikes)

        return Mean_Absolute_Error(approximations,true_values)
        
        

        
    
        
        
