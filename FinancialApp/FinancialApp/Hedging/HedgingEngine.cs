using System;
using System.Collections.Generic;
using System.Threading.Tasks;

using FinancialApp.Grpc;
using MarketData;
using ParameterInfo;
using GrpcPricing.Protos;
using FinancialApp.Hedging;
using FinancialApp.Rebalancing;
using FinancialApp.Portfolio;

namespace FinancialApp.Services
{
    public class HedgingEngine : IHedgingEngine
    {
        public void AddOutputList(List<OutputData> outputDataList, DataFeed dataFeed, PricingOutput pricingResults, Portfolio.Portfolio portfolio)
        {
            OutputData outputData = new OutputData
            {
                Date = dataFeed.Date,
                Deltas = pricingResults.Deltas.ToArray(),
                DeltasStdDev = pricingResults.DeltasStdDev.ToArray(),
                Price = pricingResults.Price,
                PriceStdDev = pricingResults.PriceStdDev,
                Value = portfolio.Value()//TODO modifie cela 
            };

            outputDataList.Add(outputData);
        }

        public List<OutputData> ComputePortfolio(TestParameters financialParams, List<DataFeed> dataFeeds)
        {
            //TODO: Compute the initialPrice with the pricer (part c++ via GRPC), initialize the portfolio 
            DataFeed initialDataFeed = dataFeeds[0];
            DateTime current_date= initialDataFeed.Date;


            //TODO: compute the output Data 
            List<OutputData> outputDatas = new List<OutputData>();
            //TODO: Ici l'abstraction n'est pas maximum car je depend du type fixedperiod
            IRebalancingOracleBase rebalancingOracle = new FixedRebalancingOracle(financialParams.RebalancingOracleDescription.Period,current_date);

            foreach (DataFeed dataFeed in dataFeeds.Skip(1))
            {
                current_date = dataFeed.Date;

                // Mise à jour du portefeuille
                portfolio.UpdatePortfolio(portfolio, dataFeed, currentPrices, testParameters);

                // Vérification du rebalancement via l'oracle
                if (rebalancingOracle.ShouldRebalance(current_date))
                {
                    //todo appeler a nouveau le pricer, puius updtae le porfolio en le mettant à jour avec sa nouvelele compo en utilisant la metho dans optionAsset.  enfin ajoute la methode update porfolio dans le portfolio. 
                }

            
            throw new NotImplementedException();
        }
    }
}
