﻿using GrpcPricing.Protos;
using MarketData;
using ParameterInfo;
namespace FinancialApp.Hedging
{
    public interface IHedgingEngine
    {
        public List<OutputData> ComputePortfolio(TestParameters financialParams, List<DataFeed> dataFeeds);
        public  void AddOutputList(List<OutputData> outputDataList, DataFeed dataFeed, PricingOutput pricingResults, Portfolio.Portfolio portfolio);
    }
}