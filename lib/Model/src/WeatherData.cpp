#include <ArduinoJson.h>
#include "WeatherData.h"
#include "Util.h"

void WeatherData::clearWeatherDataFromFlash(Preferences& preferences) {
    try {
        preferences.remove("weatherDataDOM");
    } catch (const char* e) {}
    try {
        preferences.remove("weatherData");
    } catch (const char* e) {}
}

void WeatherData::tryLoadWeatherDataFromFlash(Preferences& preferences) {
    clear();
    if(currentDayOfMonth() != preferences.getUInt("weatherDataDOM"))
        return;

    String weatherDataJson = preferences.getString("weatherData");
    Serial.print("LOADED weatherDataJson: ");
    Serial.println(weatherDataJson);
    try {
        JsonDocument doc;
        deserializeJson(doc, weatherDataJson);

        weatherDataType = stringToWeatherDataType(doc["weatherDataType"].as<String>());
        UVICurrent = doc["UVICurrent"].as<uint8_t>();
        AQICurrent = doc["AQICurrent"].as<uint16_t>();

        times.clear();
        JsonArray arrTimes = doc["times"];
        for (JsonVariant item: arrTimes)
            times.push_back(item.as<uint8_t>());

        weatherIcons.clear();
        JsonArray arrweatherIcons = doc["weatherIcons"];
        for (JsonVariant item: arrweatherIcons)
            weatherIcons.push_back(item.as<uint8_t>());

        temperaturesCelcius.clear();
        JsonArray arrTemperaturesCelcius = doc["temperaturesCelcius"];
        for (JsonVariant item: arrTemperaturesCelcius)
            temperaturesCelcius.push_back(item.as<String>());

        apparentTemperaturesCelcius.clear();
        JsonArray arrApparentTemperaturesCelcius = doc["apparentTemperaturesCelcius"];
        for (JsonVariant item: arrApparentTemperaturesCelcius)
            apparentTemperaturesCelcius.push_back(item.as<String>());

        relativeHumidities.clear();
        JsonArray arrRelativeHumidities = doc["relativeHumidities"];
        for (JsonVariant item: arrRelativeHumidities)
            relativeHumidities.push_back(item.as<String>());

        windSpeeds.clear();
        JsonArray arrWindSpeeds = doc["windSpeeds"];
        for (JsonVariant item: arrWindSpeeds)
            windSpeeds.push_back(item.as<String>());

        precipitationProbabilities.clear();
        JsonArray arrPrecipitationProbabilities = doc["precipitationProbabilities"];
        for (JsonVariant item: arrPrecipitationProbabilities)
            precipitationProbabilities.push_back(item.as<String>());

        precipitationAmounts.clear();
        JsonArray arrPrecipitationAmounts = doc["precipitationAmounts"];
        for (JsonVariant item: arrPrecipitationAmounts)
            precipitationAmounts.push_back(item.as<String>());
        
    } catch (const char* e) {
        Serial.println("EXCEPTION LOADING WEATHERDATA");
        clearWeatherDataFromFlash(preferences);
        weatherIcons.clear();
        return;
    }
}

void WeatherData::saveWeatherDataToFlash(Preferences& preferences) {
    try {
        JsonDocument doc;

        doc["weatherDataType"] = weatherDataTypeToString(weatherDataType);
        doc["UVICurrent"] = UVICurrent;
        doc["AQICurrent"] = AQICurrent;

        JsonArray arrTimes = doc["times"].to<JsonArray>();
        for(const uint8_t& time : times)
            arrTimes.add(time);
        
        JsonArray arrweatherIcons = doc["weatherIcons"].to<JsonArray>();
        for(const int& weatherIcon : weatherIcons)
            arrweatherIcons.add(weatherIcon);

        JsonArray arrTemperaturesCelcius = doc["temperaturesCelcius"].to<JsonArray>();
        for(const String& temperatureCelcius : temperaturesCelcius)
            arrTemperaturesCelcius.add(temperatureCelcius);

        JsonArray arrApparentTemperaturesCelcius = doc["apparentTemperaturesCelcius"].to<JsonArray>();
        for(const String& apparentTemperatureCelcius : apparentTemperaturesCelcius)
            arrApparentTemperaturesCelcius.add(apparentTemperatureCelcius);

        JsonArray arrRelativeHumidities = doc["relativeHumidities"].to<JsonArray>();
        for(const String& relativeHumidity : relativeHumidities)
            arrRelativeHumidities.add(relativeHumidity);

        JsonArray arrWindSpeeds = doc["windSpeeds"].to<JsonArray>();
        for(const String& windSpeed : windSpeeds)
            arrWindSpeeds.add(windSpeed);

        JsonArray arrPrecipitationProbabilities = doc["precipitationProbabilities"].to<JsonArray>();
        for(const String& precipitationProbability : precipitationProbabilities)
            arrPrecipitationProbabilities.add(precipitationProbability);

        JsonArray arrPrecipitationAmount = doc["precipitationAmounts"].to<JsonArray>();
        for(const String& precipitationAmount : precipitationAmounts)
            arrPrecipitationAmount.add(precipitationAmount);

        String weatherDataJson;
        serializeJson(doc, weatherDataJson);
        Serial.print("SAVING WeatherData JSON: ");
        Serial.println(weatherDataJson);
        preferences.putString("weatherData", weatherDataJson);
        preferences.putUInt("weatherDataDOM", currentDayOfMonth());

    } catch (const char* e) {
        Serial.println("EXCEPTION SAVING WEATHERDATA");
        clearWeatherDataFromFlash(preferences);
        return;
    }
}

void WeatherData::clear() {
    UVICurrent = 0;
    AQICurrent = 0;
    times.clear();
    weatherIcons.clear();
    temperaturesCelcius.clear();
    apparentTemperaturesCelcius.clear();
    relativeHumidities.clear();
    windSpeeds.clear();
    precipitationProbabilities.clear();
    precipitationAmounts.clear();
}

bool WeatherData::isStaleOrInvalid(WeatherDataType requiredWeatherDataType) {
    return requiredWeatherDataType != weatherDataType || times.size() == 0 || currentHourOfDay() != times[0];
}

String WeatherData::weatherDataTypeToString(WeatherDataType weatherDataType) {
    if(weatherDataType == VickyCommuteForecast)
        return "VickyCommuteForecast";
    else
        return "ThreeHourForecast";
}

WeatherDataType WeatherData::stringToWeatherDataType(String weatherDataTypeStr) {
    if(weatherDataTypeStr == "VickyCommuteForecast")
        return VickyCommuteForecast;
    else
        return ThreeHourForecast;
}