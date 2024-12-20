using GrpcPricing.Protos;
using MarketData;
using ParameterInfo;
using FinancialApp.Portfolios;
using FinancialApp.Grpc;
namespace FinancialApp.Hedging
{
    public interface IHedgingEngine
    {
        public Task<List<OutputData>> ComputePortfolio(TestParameters financialParams, List<DataFeed> dataFeeds);
        public  void AddOutputList(List<OutputData> outputDataList, DataFeed dataFeed, PriceEstimation pricingResults, Portfolio portfolio);
    }
}
