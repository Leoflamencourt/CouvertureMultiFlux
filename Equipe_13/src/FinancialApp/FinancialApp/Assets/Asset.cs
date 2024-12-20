using System;

namespace FinancialApp.Assets
{
    public  class Asset
    {
        public String Id { get; set; }  // Propriété publique pour être accessible
        public double MarketPrice { get; set; }
        public DateTime Date { get; set; }
        

        public Asset(string id, double price, DateTime date)
        {
            Id = id;
            MarketPrice = price;
            Date = date;
          
        }
    }
}
