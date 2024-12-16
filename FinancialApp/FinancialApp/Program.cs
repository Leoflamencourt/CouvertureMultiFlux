using System;
using System.Threading.Tasks;
using FinancialApp.Grpc;
using FinancialApp.Services;
using FinancialApp.Deserializer;
using FinancialApp.Serializer;
using FinancialApp.Serializers;
using FinancialApp.Parser;

class Program
{
    static async Task Main(string[] args)
    {
        // Vérifier que les arguments de la ligne de commande sont fournis
        if (args.Length < 2)
        {
            Console.WriteLine("Usage: FinancialApp <marketDataPath> <financialParamsPath>");
            return;
        }

        string marketDataPath = args[0];
        string financialParamsPath = args[1];

        // Initialisation des dépendances
        var grpcClient = new GrpcPricingClient("http://localhost:50051");
        var marketDataDeserializer = new MarketDataDeserializer();
        var financialParamsDeserializer = new ParameterJsonDeserialiser();
        var pricingInputSerializer = new PricingInputSerializer();

        // Création de l'HedgingEngineClient
        var hedgingEngineClient = new HedgingEngineClient(
            grpcClient,
            marketDataDeserializer,
            financialParamsDeserializer,
            pricingInputSerializer
        );

        // Exécution du processus de couverture avec les chemins fournis
        await hedgingEngineClient.RunHedgingAsync(marketDataPath, financialParamsPath);
    }
}
