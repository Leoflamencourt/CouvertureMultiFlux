using FinancialApp.Assets;
using FinancialApp.Grpc;
using GrpcPricing.Protos;
using MarketData;
using ParameterInfo;
using Utils;

namespace FinancialApp.Portfolios
{
    public class Portfolio
    {
        public Dictionary<string, double> AssetProportions { get; private set; }
        public double Cash { get; private set; }
        public DateTime LastRebalancingDate { get; private set; }

        public Portfolio(DataFeed actualPrices, double initialInvestment, Dictionary<string, double> assetProportions, TestParameters parameters, DateTime rebalancingDate)
        {
            
            this.AssetProportions = assetProportions;
            double cash = initialInvestment;

            // Initialiser les proportions et calculer le cash restant
            foreach (var asset  in assetProportions.Keys)
            {
                double assetValue = assetProportions[asset] * actualPrices.SpotList[asset];
                cash -= assetValue;
            }

            this.Cash = cash;
            this.LastRebalancingDate = actualPrices.Date;
        }

        public double Value(Dictionary<String, double> ListPrices)
        {
            var result = Cash;
            foreach (var asset in ListPrices.Keys)
            {
                result += ListPrices[asset]*AssetProportions[asset];
            }
            return result;
        }

        public void UpdatePortfolio(Portfolio portfolio, DataFeed dataFeed, PriceEstimation pricingResults, TestParameters financialParams)
        {
            // Mettre à jour les proportions des actifs selon les nouveaux prix
            Dictionary<string, double> updatedProportions = new Dictionary<string, double>();

            foreach (var asset in portfolio.AssetProportions.Keys)
            {
                if (dataFeed.SpotList.ContainsKey(asset))
                {
                    // Mettre à jour la proportion en fonction des nouveaux prix
                    double updatedProportion = portfolio.AssetProportions[asset] * dataFeed.SpotList[asset] / pricingResults.Price;
                    updatedProportions[asset] = updatedProportion;
                }
                else
                {
                    // Si l'actif n'est pas dans le SpotList, conserver l'ancienne proportion
                    updatedProportions[asset] = portfolio.AssetProportions[asset];
                }
            }

            // Calculer la nouvelle valeur du portefeuille
            double portfolioValue = portfolio.Value(dataFeed.SpotList);

            // Mettre à jour le cash disponible après réévaluation
            double updatedCash = portfolio.Cash + (pricingResults.Price - portfolioValue);

            // Mettre à jour le portefeuille
            portfolio.AssetProportions = updatedProportions;
            portfolio.Cash = updatedCash;
            portfolio.LastRebalancingDate = dataFeed.Date;
        }


        public void Rebalance(Dictionary<String, double> newAssetProportion, TestParameters parameters, DataFeed actualPrices)
        {
            double newCash = 0;

            foreach (var asset in AssetProportions.Keys)
            {
                newCash += actualPrices.SpotList[asset] * (AssetProportions[asset] - newAssetProportion[asset]);
            }

            Cash = newCash + Util.CapitalizedCash(Cash, LastRebalancingDate, actualPrices.Date, parameters, "eur");
            AssetProportions = newAssetProportion;
            LastRebalancingDate = actualPrices.Date;
        }

    }
}
