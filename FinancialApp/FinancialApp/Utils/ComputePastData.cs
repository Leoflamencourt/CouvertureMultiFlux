// Classe dédiée au calcul des données historiques
using System;
using System.Collections.Generic;
using GrpcPricing.Protos;
using MarketData;
using ParameterInfo;

namespace FinancialApp.Utils
{
    public class ComputePastData
    {
        public static List<DataFeed> ComputePastLines(List<DataFeed> subdataFeeds, TestParameters parameters)
        {
            List<DataFeed> past = new List<DataFeed>();

            for (int i = 0; i < subdataFeeds.Count - 1; ++i)
            {
                DataFeed dataFeed = subdataFeeds[i];
                if (parameters.PayoffDescription.PaymentDates.Contains(dataFeed.Date)) { 
                    past.Add(dataFeed);
                
                }
            }



            past.Add(subdataFeeds.Last());
            return past;
        }
    }
}
