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
using System.Globalization;

namespace FinancialApp.Services
{
    public class HedgingEngine : IHedgingEngine
    {
        public void AddOutputList(List<OutputData> outputDataList, DataFeed dataFeed, PriceEstimation pricingResults, Portfolio portfolio)
        {
            OutputData outputData = new OutputData
            {
                Date = dataFeed.Date,
                Deltas = pricingResults.Deltas.Values.ToArray(),
                DeltasStdDev = pricingResults.DeltaStdDevs.Values.ToArray(),
                Price = pricingResults.Price,
                PriceStdDev = pricingResults.PriceStdDev,
                Value = portfolio.Value(dataFeed.SpotList)
            };

            outputDataList.Add(outputData);
        }

        public async Task<List<OutputData>> ComputePortfolio(TestParameters financialParams, List<DataFeed> dataFeeds)
        {
            if (dataFeeds == null || dataFeeds.Count == 0)
                throw new ArgumentException("DataFeeds cannot be null or empty.");

            DataFeed initialDataFeed = dataFeeds[0];
            DateTime currentDate = initialDataFeed.Date;

            // Appeler le pricer pour obtenir les valeurs initiales de prix et de delta
            Pricer pricer = new Pricer();
            PriceEstimation initialPricingResults = await pricer.PriceandDeltaAsync(new List<DataFeed> { initialDataFeed }, currentDate, financialParams);

            // Calculer l'investissement initial restant : prix - delta * spot initial
            double initialInvestment = initialPricingResults.Price;
            Dictionary<string, double> initialProportions = initialPricingResults.Deltas;
            foreach (var assetId in initialPricingResults.Deltas.Keys)
            {
                initialInvestment -= initialPricingResults.Deltas[assetId] * initialDataFeed.SpotList[assetId];
            }

            // Initialiser le portefeuille
            Portfolio portfolio = new Portfolio(initialDataFeed, initialInvestment, initialProportions, financialParams, currentDate);

            // Initialiser l'oracle de rebalancement
            IRebalancingOracleBase rebalancingOracle = new FixedRebalancingOracle(financialParams.RebalancingOracleDescription.Period, currentDate);

            List<OutputData> outputDataList = new List<OutputData>();
            PriceEstimation lastPricingResults = initialPricingResults;

            foreach (DataFeed dataFeed in dataFeeds.Skip(1))
            {
                currentDate = dataFeed.Date;

                // Vérifier si un rebalancement est nécessaire
                if (rebalancingOracle.ShouldRebalance(currentDate))
                {
                    // Appeler le pricer pour obtenir les nouvelles valeurs de prix et de delta
                    var subDataFeeds = dataFeeds.Where(df => df.Date <= currentDate).ToList();
                    lastPricingResults = await pricer.PriceandDeltaAsync(subDataFeeds, currentDate, financialParams);

                    // Créer ou mettre à jour un objet OptionAsset
                    OptionAsset optionAsset = new OptionAsset(lastPricingResults.Price, lastPricingResults.PriceStdDev, lastPricingResults.Deltas, lastPricingResults.DeltaStdDevs);

                    // Mettre à jour les deltas
                    optionAsset.UpdateDeltas(lastPricingResults.Deltas);

                    // Rebalancer le portefeuille
                    portfolio.Rebalance(optionAsset.Deltas, financialParams, dataFeed);
                }

                // Mettre à jour le portefeuille
                portfolio.UpdatePortfolio(portfolio, dataFeed, lastPricingResults, financialParams);

                // Ajouter les données de sortie
                AddOutputList(outputDataList, dataFeed, lastPricingResults, portfolio);
            }


            return outputDataList;
        }

    }
}
