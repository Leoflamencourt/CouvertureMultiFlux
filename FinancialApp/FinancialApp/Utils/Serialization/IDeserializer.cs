namespace FinancialApp.Utils.Serialization
{
    public interface IDeserializer<T>
    {
        T Deserialize(string filePath);
    }
}
