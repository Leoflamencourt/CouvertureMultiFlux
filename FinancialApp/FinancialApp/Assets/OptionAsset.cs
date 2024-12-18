namespace FinancialApp.Assets
{
    /// <summary>
    /// Classe représentant une option avec ses propriétés spécifiques.
    /// </summary>
    public class OptionAsset : AssetEstimation
    {
        public List<double> Deltas { get; set; }
        public List<double> DeltasStdDev { get; set; }

        public OptionAsset(double price, double priceStdDev, List<double> deltas, List<double> deltasStdDev)
            : base(price, priceStdDev)
        {
            Deltas = deltas;
            DeltasStdDev = deltasStdDev;
        }

      
        public override Dictionary<string, double> GetUpdatedProportions()
        {
            var updatedProportions = new Dictionary<string, double>();

            for (int i = 0; i < Deltas.Count; i++)
            {
                string assetId = $"Asset{i + 1}";
                updatedProportions[assetId] = Deltas[i]; // Quantité basée sur les deltas
            }

            return updatedProportions;
        }
    }

}