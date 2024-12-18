
using MarketData;
using ParameterInfo;

namespace Utils
{
    /// <summary>
    /// Class containing useful functions to improve code readability.
    /// </summary>
    /// <remarks> This class contains the methods for computing a scalar product and accessing 
    /// the asset's spots for a given datafeed.
    /// </remarks>
    public static class Util
    {
        /// <summary>
        /// Computes the scalar product between two lists of doubles.
        /// </summary>
        /// <returns>A double with the result of the operation.</returns>
        public static double ScalarProduct(double[] listA, double[] listB, int nbElements)
        {
            double result = 0;
            for (var i = 0; i < nbElements; i++) { result = result + listA[i] * listB[i]; }
            return result;
        }

        public static double[] GetComposition(string[] shareIds, Dictionary<String, double> Assets)
        {
            var Composition = new double[Assets.Count];
            var Counter = 0;
            foreach (var shareId in shareIds)
            {
                Composition[Counter] = Assets[shareId];
                Counter++;
            }
            return Composition;
        }





        /// <summary>
        /// Computes the new value for the cash since the last rebalancing
        /// </summary>
        /// <returns> double representing the new cash value. </returns>
        public static double CapitalizedCash(double oldCash, DateTime lastRebalancingDate, DateTime currentDate, TestParameters testParameters, string devise)
        {
            double daysPerYears = testParameters.NumberOfDaysInOneYear;
            double dayswork= TimeHandler.DayCount.CountBusinessDays(lastRebalancingDate,currentDate);
            double riskFreeRate = testParameters.AssetDescription.CurrencyRates[devise];
            var appreciatedValue =dayswork/daysPerYears;
            
            return oldCash * appreciatedValue * riskFreeRate;
        }


    }

}
