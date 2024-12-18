using System;
using System.Collections.Generic;
using System.Threading.Tasks;

using FinancialApp.Grpc;
using MarketData;
using ParameterInfo;
using GrpcPricing.Protos;
using FinancialApp.Hedging;
using FinancialApp.Rebalancing;
using FinancialApp.Portfolios;
using FinancialApp.Assets;

namespace FinancialApp.Services
{
    public class HedgingEngine : IHedgingEngine
    {
        public void AddOutputList(List<OutputData> outputDataList, DataFeed dataFeed, PricingOutput pricingResults, Portfolio portfolio)
        {
            OutputData outputData = new OutputData
            {
                Date = dataFeed.Date,
                Deltas = pricingResults.Deltas.ToArray(),
                DeltasStdDev = pricingResults.DeltasStdDev.ToArray(),
                Price = pricingResults.Price,
                PriceStdDev = pricingResults.PriceStdDev,
                Value = portfolio.Value(dataFeed.SpotList)
            };

            outputDataList.Add(outputData);
        }

        public List<OutputData> ComputePortfolio(TestParameters financialParams, List<DataFeed> dataFeeds)
        {
            if (dataFeeds == null || dataFeeds.Count == 0)
                throw new ArgumentException("DataFeeds cannot be null or empty.");

            DataFeed initialDataFeed = dataFeeds[0];
            DateTime currentDate = initialDataFeed.Date;

            // Appeler le pricer pour obtenir les valeurs initiales de prix et de delta
            Pricer pricer = new Pricer();
            PricingOutput initialPricingResults = pricer.PriceandDeltaAsync(new List<DataFeed> { initialDataFeed }, currentDate, financialParams).Result;

            // Calculer les proportions initiales basées sur les deltas
            Dictionary<string, double> initialProportions = new Dictionary<string, double>();
            for (int i = 0; i < initialPricingResults.Deltas.Count; i++)
            {
                string assetId = $"Asset{i + 1}";
                initialProportions[assetId] = initialPricingResults.Deltas[i];
            }

            // Calculer l'investissement initial restant : prix - delta * spot initial
            double initialInvestment = initialPricingResults.Price;
            foreach (var assetId in initialProportions.Keys)
            {
                initialInvestment -= initialProportions[assetId] * initialDataFeed.SpotList[assetId];
            }

            // Initialiser le portefeuille
            Portfolio portfolio = new Portfolio(initialDataFeed, initialInvestment, initialProportions, financialParams, currentDate);

            // Initialiser l'oracle de rebalancement
            IRebalancingOracleBase rebalancingOracle = new FixedRebalancingOracle(financialParams.RebalancingOracleDescription.Period, currentDate);

            List<OutputData> outputDataList = new List<OutputData>();
            PricingOutput lastPricingResults = initialPricingResults;

            foreach (DataFeed dataFeed in dataFeeds.Skip(1))
            {
                currentDate = dataFeed.Date;

                // Vérifier si un rebalancement est nécessaire
                if (rebalancingOracle.ShouldRebalance(currentDate))
                {
                    // Appeler le pricer pour obtenir les nouvelles valeurs de prix et de delta
                    var subDataFeeds = dataFeeds.Where(df => df.Date <= currentDate).ToList();
                    lastPricingResults = pricer.PriceandDeltaAsync(subDataFeeds, currentDate, financialParams).Result;

                    // Todo modifier cela pour ne pas avoir à changer a appaler option asst qui n'est pa sun type géneriue 
                    OptionAsset optionAsset = new OptionAsset(lastPricingResults.Price, lastPricingResults.PriceStdDev, lastPricingResults.Deltas.ToList(), lastPricingResults.DeltasStdDev.ToList());
                    Dictionary<string, double> newProportions = optionAsset.GetUpdatedProportions();

                    // Rebalancer le portefeuille
                    portfolio.Rebalance(newProportions, financialParams, dataFeed);
                }

                
                portfolio.UpdatePortfolio(portfolio, dataFeed, lastPricingResults, financialParams);

                
                AddOutputList(outputDataList, dataFeed, lastPricingResults, portfolio);
            }

            return outputDataList;
        }

    }
}
