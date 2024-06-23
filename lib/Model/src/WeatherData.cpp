#include <ArduinoJson.h>
#include "WeatherData.h"
#include "Util.h"

void WeatherData::tryLoadWeatherDataFromFlash(Preferences& preferences) {
    clear();

    uint8_t weatherDataSavedDOM = preferences.getUInt("weatherDataDom");
    Serial.print("weatherDataSavedDOM is ");
    Serial.println(weatherDataSavedDOM);
    
    if(currentDayOfMonth() != weatherDataSavedDOM) {
        Serial.println("BAD, INNEO");
        return;
    }

    Serial.println("GOOD, loading");
    String weatherDataJson = preferences.getString("weatherData");
    Serial.print("weatherDataJson is ");
    Serial.println(weatherDataJson);

    try {
        JsonDocument doc;
        deserializeJson(doc, weatherDataJson);

        weatherDataType = stringToWeatherDataType(doc["weatherDataType"]);
        UVICurrent = doc["UVICurrent"];
        AQICurrent = doc["AQICurrent"];

        times.clear();
        JsonArray arrTimes = doc["times"];
        for (JsonVariant item: arrTimes)
            times.push_back(item.as<uint8_t>());

        isDaytime.clear();
        JsonArray arrDaytime = doc["isDaytime"];
        for (JsonVariant item: arrDaytime)
            isDaytime.push_back(item.as<bool>());

        weatherType.clear();
        JsonArray arrWeatherType = doc["weatherType"];
        for (JsonVariant item: arrWeatherType)
            weatherType.push_back(stringToWeatherType(item.as<String>()));

        temperatureCelcius.clear();
        JsonArray arrTemperatureCelcius = doc["temperatureCelcius"];
        for (JsonVariant item: arrTemperatureCelcius)
            temperatureCelcius.push_back(item.as<String>());

        apparentTemperatureCelcius.clear();
        JsonArray arrApparentTemperatureCelcius = doc["apparentTemperatureCelcius"];
        for (JsonVariant item: arrApparentTemperatureCelcius)
            apparentTemperatureCelcius.push_back(item.as<String>());

        relativeHumidity.clear();
        JsonArray arrRelativeHumidity = doc["relativeHumidity"];
        for (JsonVariant item: arrRelativeHumidity)
            relativeHumidity.push_back(item.as<String>());

        windSpeed.clear();
        JsonArray arrWindSpeed = doc["windSpeed"];
        for (JsonVariant item: arrWindSpeed)
            windSpeed.push_back(item.as<String>());

        precipitationProbability.clear();
        JsonArray arrPrecipitationProbability = doc["precipitationProbability"];
        for (JsonVariant item: arrPrecipitationProbability)
            precipitationProbability.push_back(item.as<String>());

        precipitation.clear();
        JsonArray arrPrecipitation = doc["precipitation"];
        for (JsonVariant item: arrPrecipitation)
            precipitation.push_back(item.as<String>());
        
    } catch (const char* e) {
        Serial.println("EXCEPTION LOADING WEATHERDATA");
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

        JsonArray arrDaytime = doc["isDaytime"].to<JsonArray>();
        for(const bool& isDaytime : isDaytime)
            arrDaytime.add(isDaytime);
        
        JsonArray arrWeatherType = doc["weatherType"].to<JsonArray>();
        for(const WeatherType& weatherType : weatherType)
            arrWeatherType.add(weatherTypeToString(weatherType));

        JsonArray arrTemperatureCelcius = doc["temperatureCelcius"].to<JsonArray>();
        for(const String& temperatureCelcius : temperatureCelcius)
            arrTemperatureCelcius.add(temperatureCelcius);

        JsonArray arrApparentTemperatureCelcius = doc["apparentTemperatureCelcius"].to<JsonArray>();
        for(const String& apparentTemperatureCelcius : apparentTemperatureCelcius)
            arrApparentTemperatureCelcius.add(apparentTemperatureCelcius);

        JsonArray arrRelativeHumidity = doc["relativeHumidity"].to<JsonArray>();
        for(const String& relativeHumidity : relativeHumidity)
            arrRelativeHumidity.add(relativeHumidity);

        JsonArray arrWindSpeed = doc["windSpeed"].to<JsonArray>();
        for(const String& windSpeed : windSpeed)
            arrWindSpeed.add(windSpeed);

        JsonArray arrPrecipitationProbability = doc["precipitationProbability"].to<JsonArray>();
        for(const String& precipitationProbability : precipitationProbability)
            arrPrecipitationProbability.add(precipitationProbability);

        JsonArray arrPrecipitation = doc["precipitation"].to<JsonArray>();
        for(const String& precipitation : precipitation)
            arrPrecipitation.add(precipitation);

        String weatherDataJson;
        serializeJson(doc, weatherDataJson);
        Serial.print("SAVING WeatherData JSON: ");
        Serial.println(weatherDataJson);
        preferences.putString("weatherData", weatherDataJson);
        preferences.putUInt("weatherDataDom", currentDayOfMonth());

    } catch (const char* e) {
        return;
    }
}

void WeatherData::clear() {
    UVICurrent = 0;
    AQICurrent = 0;
    times.clear();
    isDaytime.clear();
    weatherType.clear();
    temperatureCelcius.clear();
    apparentTemperatureCelcius.clear();
    relativeHumidity.clear();
    windSpeed.clear();
    precipitationProbability.clear();
    precipitation.clear();
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


String WeatherData::weatherTypeToString(WeatherType weatherType) {
    switch (weatherType) {
        case Clear: return "Clear";
        case MainlyClear: return "MainlyClear";
        case PartlyCloudy: return "PartlyCloudy";
        case Overcast: return "Overcast";
        case Fog: return "Fog";
        
        case Drizzle: return "Drizzle";
        case FreezingDrizzle: return "FreezingDrizzle";
        
        case SlightRain: return "SlightRain";
        case ModerateRain: return "ModerateRain";
        case HeavyRain: return "HeavyRain";

        case LightFreezingRain: return "LightFreezingRain";
        case HeavyFreezingRain: return "HeavyFreezingRain";

        case LightSnow: return "LightSnow";
        case ModerateSnow: return "ModerateSnow";
        case HeavySnow: return "HeavySnow";
        case SnowGrains: return "SnowGrains";

        case SlightRainShowers: return "SlightRainShowers";
        case ModerateRainShowers: return "ModerateRainShowers";
        case ViolentRainShowers: return "ViolentRainShowers";

        case SlightSnowShowers: return "SlightSnowShowers";
        case HeavySnowShowers: return "HeavySnowShowers";

        case Thunderstorm: return "Thunderstorm";
        
        case Unknown: 
        default:
            return "Unknown";
    }
}

WeatherType WeatherData::stringToWeatherType(String weatherTypeStr) {
    if (weatherTypeStr == "Clear") {
        return Clear;
    } else if (weatherTypeStr == "MainlyClear") {
        return MainlyClear;
    } else if (weatherTypeStr == "PartlyCloudy") {
        return PartlyCloudy;
    } else if (weatherTypeStr == "Overcast") {
        return Overcast;
    } else if (weatherTypeStr == "Fog") {
        return Fog;
    } else if (weatherTypeStr == "Drizzle") {
        return Drizzle;
    } else if (weatherTypeStr == "FreezingDrizzle") {
        return FreezingDrizzle;
    } else if (weatherTypeStr == "SlightRain") {
        return SlightRain;
    } else if (weatherTypeStr == "ModerateRain") {
        return ModerateRain;
    } else if (weatherTypeStr == "HeavyRain") {
        return HeavyRain;
    } else if (weatherTypeStr == "LightFreezingRain") {
        return LightFreezingRain;
    } else if (weatherTypeStr == "HeavyFreezingRain") {
        return HeavyFreezingRain;
    } else if (weatherTypeStr == "LightSnow") {
        return LightSnow;
    } else if (weatherTypeStr == "ModerateSnow") {
        return ModerateSnow;
    } else if (weatherTypeStr == "HeavySnow") {
        return HeavySnow;
    } else if (weatherTypeStr == "SnowGrains") {
        return SnowGrains;
    } else if (weatherTypeStr == "SlightRainShowers") {
        return SlightRainShowers;
    } else if (weatherTypeStr == "ModerateRainShowers") {
        return ModerateRainShowers;
    } else if (weatherTypeStr == "ViolentRainShowers") {
        return ViolentRainShowers;
    } else if (weatherTypeStr == "SlightSnowShowers") {
        return SlightSnowShowers;
    } else if (weatherTypeStr == "HeavySnowShowers") {
        return HeavySnowShowers;
    } else if (weatherTypeStr == "Thunderstorm") {
        return Thunderstorm;
    } else {
        return Unknown;
    }
}