using GrpcPricing.Protos;
using MarketData;
using ParameterInfo;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FinancialApp.Grpc
{
    public interface IPricer
    {
        public PricingOutput Price(DateTime current_date, List<DataFeed> dataFeeds, TestParameters testParameters);
    }
}
