using System.Globalization;

namespace FinancialApp.Assets
{
    /// <summary>
    /// Classe représentant une option avec ses propriétés spécifiques.
    /// </summary>
    public class OptionAsset : AssetEstimation
    {
        public Dictionary<string, double> Deltas { get; set; }
        public Dictionary<string, double> DeltasStdDev { get; set; }

        public OptionAsset(double price, double priceStdDev, Dictionary<string, double> deltas, Dictionary<string, double> deltasStdDev)
            : base(price, priceStdDev)
        {
            Deltas = deltas;
            DeltasStdDev = deltasStdDev;
        }


        public void UpdateDeltas(Dictionary<string, double> newDeltas)
        {
            Deltas = newDeltas;
        }
    }

}