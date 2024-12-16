using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using FinancialApp.Grpc;
using FinancialApp.Deserializer;
using FinancialApp.Serializer;
using MarketData;
using ParameterInfo;
using GrpcPricing.Protos;

namespace FinancialApp.Services
{
    public class HedgingEngineClient : IHedgingEngineClient
    {
        private readonly IGrpcPricingClient _grpcClient;
        private readonly IInputMarketDataDeserializer _marketDataDeserializer;
        private readonly IInputParametersJsonDeserializer _financialParamsDeserializer;
        private readonly IPricingInputSerializer _pricingInputSerializer;

        public HedgingEngineClient(
            IGrpcPricingClient grpcClient,
            IInputMarketDataDeserializer dataDeserializer,
            IInputParametersJsonDeserializer parameterDeserializer,
            IPricingInputSerializer pricingInputSerializer)
        {
            _grpcClient = grpcClient;
            _marketDataDeserializer = dataDeserializer;
            _financialParamsDeserializer = parameterDeserializer;
            _pricingInputSerializer = pricingInputSerializer;
        }

        public async Task ProcessAsync(string marketDataPath, string financialParamsPath)
        {
            // Étape 1 : Désérialiser les DataFeeds à partir du fichier CSV
            var dataFeeds = _marketDataDeserializer.Deserialize(marketDataPath);

            // Étape 2 : Sérialiser les DataFeeds en PricingInput
            var pricingInput = _pricingInputSerializer.Serialize(dataFeeds, true, DateTime.Now.ToOADate());

            // Étape 3 : Appeler le serveur gRPC avec PricingInput
            var pricingOutput = await _grpcClient.GetPriceAndDeltasAsync(pricingInput);

            // Étape 4 : Désérialiser les Financial Parameters
            var financialParams = _financialParamsDeserializer.Deserialize(financialParamsPath);

            // Étape 5 : Passer les résultats au portefeuille
           // Portfolio.UpdatePortfolio(financialParams, pricingOutput);

            Console.WriteLine("Processus de couverture terminé !");
        }

        public async Task RunHedgingAsync(string marketDataPath, string financialParamsPath)
        {
            try
            {
                Console.WriteLine("Démarrage du processus de couverture...");

                // Appeler le traitement principal avec les chemins des fichiers
                await ProcessAsync(marketDataPath, financialParamsPath);

                Console.WriteLine("Processus de couverture terminé avec succès !");
            }
            catch (Exception ex)
            {
                // Gérer les erreurs
                Console.WriteLine($"Une erreur est survenue : {ex.Message}");
            }
        }


    }
}
