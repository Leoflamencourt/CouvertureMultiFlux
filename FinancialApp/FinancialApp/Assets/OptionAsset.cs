namespace FinancialApp.Assets
{
    /// <summary>
    /// Classe représentant une option avec ses propriétés spécifiques.
    /// </summary>
    public class OptionAsset : AssetEstimation
    {
        public double[] Deltas { get; set; }
        public double[] DeltasStdDev { get; set; }

        public OptionAsset(double price, double priceStdDev, double[] deltas, double[] deltasStdDev)
            : base(price, priceStdDev)
        {
            Deltas = deltas;
            DeltasStdDev = deltasStdDev;
        }

      
        public override Dictionary<string, double> GetUpdatedProportions()
        {
            var updatedProportions = new Dictionary<string, double>();

            for (int i = 0; i < Deltas.Length; i++)
            {
                string assetId = $"Asset{i + 1}";
                updatedProportions[assetId] = Deltas[i]; // Quantité basée sur les deltas
            }

            return updatedProportions;
        }
    }

}