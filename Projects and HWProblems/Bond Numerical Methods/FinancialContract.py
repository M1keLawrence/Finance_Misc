from abc import ABC, abstractmethod
from datetime import date

class FinancialContract(ABC):
    def __init__(self, date_formed = date.today, time_to_expiry = -1/252):
        self.set_Date_formed(date_formed) 
        self.set_time_to_expiry(time_to_expiry)
        super().__init__()

    def get_Date_formed(self):
        return self.__date_formed

    def set_Date_formed(self, new_date):
        self.__date_formed = new_date

    def set_time_to_expiry(self, time_to_expiry):
        self.__date_expires = time_to_expiry

    def get_time_to_expiry(self):
        return self.__date_expires 

    formation_date = property(get_Date_formed, set_Date_formed)
    time_to_expiry = property(get_time_to_expiry, set_time_to_expiry)

    @abstractmethod
    def Print(self):
        pass