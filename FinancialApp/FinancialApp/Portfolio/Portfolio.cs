using FinancialApp.Assets;
using ParameterInfo;
using Utils;

namespace FinancialApp.Portfolio
{
    public class Portfolio
    {
        public List<Asset> Assets { get; private set; }
        public Dictionary<string, double> AssetProportions { get; private set; }
        public double Cash { get; private set; }
        public DateTime LastRebalancingDate { get; private set; }

        public Portfolio(List<Asset> assets, double initialInvestment, Dictionary<string, double> assetProp, TestParameters parameters, DateTime rebalancingDate)
        {
            this.Assets = assets;
            this.AssetProportions = assetProp;
            double cash = initialInvestment;

            // Initialiser les proportions et calculer le cash restant
            foreach (Asset asset in assets)
            {
                double assetValue = asset.MarketPrice * AssetProportions[asset.Id];
                cash -= assetValue;
            }

            this.Cash = cash;
            this.LastRebalancingDate = assets[0].Date;
        }

        public double Value()
        {
            double totalValue = Cash;

            foreach (Asset asset in Assets)
            {
                totalValue += asset.MarketPrice * AssetProportions[asset.Id];
            }

            return totalValue;
        }

        public void Rebalance(Dictionary<String, double> newAssetProportion, TestParameters parameters)
        {
            double newCash = 0;

            foreach (Asset asset in Assets)
            {
                newCash += asset.MarketPrice * (AssetProportions[asset.Id] - newAssetProportion[asset.Id]);
            }

            Cash = newCash + Util.CapitalizedCash(Cash, LastRebalancingDate, Assets[0].Date, parameters, "eur");
            AssetProportions = newAssetProportion;
            LastRebalancingDate = Assets[0].Date;
        }

    }
}
