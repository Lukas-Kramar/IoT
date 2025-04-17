import { SensorState } from "../types/customTypes"

export interface TemperatureData {
    timeStamp: number,
    temperature: number,
    state: SensorState
}