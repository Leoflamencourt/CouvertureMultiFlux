using FinancialApp.Portfolios;
using System.Diagnostics;
using System.Xml.Linq;

namespace FinancialApp.Assets
{
    public abstract class AssetEstimation 
    {
        public double Price { get; set; }
        public double PriceStdDev { get; set; }

        public AssetEstimation( double price, double priceStdDev)
            
        {
            Price = price;
            PriceStdDev = priceStdDev;
        }

       

    }
}
