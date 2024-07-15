#include <ArduinoJson.h>
#include "forecast/Forecast.h"
#include "Util.h"



bool Forecast::isStaleOrInvalid() {
    xSemaphoreTake(mutex, portMAX_DELAY);
    bool result = hourlyConditions.size() == 0 || currentHourOfDay() != hourlyConditions[0].hour;
    xSemaphoreGive(mutex);
    return result;
}

bool Forecast::tryLoadForecastFromFlash(Preferences& preferences) {
    xSemaphoreTake(mutex, portMAX_DELAY);
    clear();
    if(currentDayOfMonth() != preferences.getUInt("forecastDOM")) {
        xSemaphoreGive(mutex);
        return false;
    }

    String forecastJson = preferences.getString("forecastData");
    Serial.print("LOADED forecastJson: ");
    Serial.println(forecastJson);
    try {
        JsonDocument doc;
        deserializeJson(doc, forecastJson);

        UVICurrent = doc["UVICurrent"].as<uint8_t>();
        AQICurrent = doc["AQICurrent"].as<uint16_t>();

        hourlyConditions.clear();
        JsonArray arrHourlyConditions = doc["hourlyConditions"];
        for (JsonVariant item: arrHourlyConditions) {
            hourlyConditions.push_back(
                HourlyCondition(
                    item["hour"].as<uint8_t>(),
                    item["weatherIcon"].as<uint8_t>(),
                    item["temperatureCelcius"].as<String>(),
                    item["apparentTemperatureCelcius"].as<String>(),
                    item["relativeHumidity"].as<String>(),
                    item["windSpeed"].as<String>(),
                    item["precipitationProbability"].as<String>(),
                    item["precipitationAmount"].as<String>()
                    )
                );
        }
        xSemaphoreGive(mutex);
        return true;
    } catch (const char* e) {
        Serial.println("EXCEPTION LOADING FORECAST");
        clearForecastFromFlash(preferences);
        clear();
        xSemaphoreGive(mutex);
        return false;
    }
}

void Forecast::saveForecastToFlash(Preferences& preferences) {
    xSemaphoreTake(mutex, portMAX_DELAY);
    try {
        JsonDocument doc;

        doc["UVICurrent"] = UVICurrent;
        doc["AQICurrent"] = AQICurrent;


        JsonArray hourlyConditionsArr = doc["hourlyConditions"].to<JsonArray>();
        for(const auto& hourlyCondition : hourlyConditions) {
            JsonObject obj = hourlyConditionsArr.add<JsonObject>();
            obj["hour"] = hourlyCondition.hour;
            obj["weatherIcon"] = hourlyCondition.weatherIcon;
            obj["temperatureCelcius"] = hourlyCondition.temperatureCelcius;
            obj["apparentTemperatureCelcius"] = hourlyCondition.apparentTemperatureCelcius;
            obj["relativeHumidity"] = hourlyCondition.relativeHumidity;
            obj["windSpeed"] = hourlyCondition.windSpeed;
            obj["precipitationProbability"] = hourlyCondition.precipitationProbability;
            obj["precipitationAmount"] = hourlyCondition.precipitationAmount;
        }

        String forecastJson;
        serializeJson(doc, forecastJson);
        Serial.print("SAVING Forecast JSON: ");
        Serial.println(forecastJson);
        preferences.putString("forecastData", forecastJson);
        preferences.putUInt("forecastDOM", currentDayOfMonth());

    } catch (const char* e) {
        Serial.println("EXCEPTION SAVING FORECAST");
        clearForecastFromFlash(preferences);
        return;
    }
    xSemaphoreGive(mutex);
}

void Forecast::updateAQI(uint16_t newAQI) {
    xSemaphoreTake(mutex, portMAX_DELAY);
    AQICurrent = newAQI;
    xSemaphoreGive(mutex);
}

void Forecast::updateUVI(uint8_t newUVI) {
    xSemaphoreTake(mutex, portMAX_DELAY);
    UVICurrent = newUVI;
    xSemaphoreGive(mutex);
}

void Forecast::updateHourlyConditions(std::vector<HourlyCondition> newHourlyConditions) {
    xSemaphoreTake(mutex, portMAX_DELAY);
    hourlyConditions.clear();
    hourlyConditions = newHourlyConditions;
    xSemaphoreGive(mutex);
}

UIForecast Forecast::getUIForecast() {
    xSemaphoreTake(mutex, portMAX_DELAY);
    UIForecast uiForecast;
    uiForecast.UVICurrent = UVICurrent;
    uiForecast.AQICurrent = AQICurrent;
    uiForecast.hourlyConditions = hourlyConditions;
    xSemaphoreGive(mutex);
    return uiForecast;
}

void Forecast::clearForecastFromFlash(Preferences& preferences) {
    try {
        preferences.remove("forecastDOM");
    } catch (const char* e) {}
    try {
        preferences.remove("forecastData");
    } catch (const char* e) {}
}

void Forecast::clear() {
    UVICurrent = 0;
    AQICurrent = 0;
    hourlyConditions.clear();
}