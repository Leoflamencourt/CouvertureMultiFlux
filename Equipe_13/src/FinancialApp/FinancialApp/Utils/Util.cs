
using GrpcPricing.Protos;
using MarketData;
using ParameterInfo;
using TimeHandler;

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

        public static Dictionary<string, double> TransformDelta(Dictionary<string, int> idPositions, PricingOutput pricingOutput)
        {
            // Créer un dictionnaire pour stocker les deltas associés à leurs identifiants
            Dictionary<string, double> deltaDictionary = new Dictionary<string, double>();

            foreach (var idPosition in idPositions)
            {
                string id = idPosition.Key;
                int position = idPosition.Value;

                if (position >= 0 && position < pricingOutput.Deltas.Count)
                {
                    deltaDictionary[id] = pricingOutput.Deltas[position];
                }
            }

            return deltaDictionary;
        }

        public static Dictionary<string, double> TransformDeltaStdDev(Dictionary<string, int> idPositions, PricingOutput pricingOutput)
        {
            // Créer un dictionnaire pour stocker les DeltaStdDev associés à leurs identifiants
            Dictionary<string, double> deltaStdDevDictionary = new Dictionary<string, double>();

            foreach (var idPosition in idPositions)
            {
                string id = idPosition.Key;
                int position = idPosition.Value;

                if (position >= 0 && position < pricingOutput.DeltasStdDev.Count)
                {
                    deltaStdDevDictionary[id] = pricingOutput.DeltasStdDev[position];
                }
            }

            return deltaStdDevDictionary;
        }


        /// <summary>
        /// Computes the new value for the cash since the last rebalancing
        /// </summary>
        /// <returns> double representing the new cash value. </returns>
        public static double CapitalizedCash(double oldCash, DateTime lastRebalancingDate, DateTime currentDate, TestParameters testParameters, string devise)
        {

            int daysPerYears = testParameters.NumberOfDaysInOneYear;
            double riskFreeRate = testParameters.AssetDescription.CurrencyRates[devise];
            double totalDays= (lastRebalancingDate - currentDate).TotalDays;
            return oldCash * Math.Exp(totalDays/365 * riskFreeRate);
        }


    }

}
