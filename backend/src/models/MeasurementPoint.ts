import { ObjectId } from "mongodb";
import dayjs from "dayjs";

import { TemperatureRange } from "../validators/other_schemas/temperatureRange.schema";
import { BaseEntity } from "./BaseEntity";

export interface SenzorConfiguration {
    created: number,
    sendInterval: number,
    measureInterval: number,
    temperatureLimits: {
        // °C
        cooling: number, // if temperature is above this number => start cooling
        heating: number // if temperature is below this number => start heating
    }
}

export type MeasuredQuantity = "temperature" | "acceleration";
export interface Senzor {
    sensorId: string,
    name: string,
    quantity: MeasuredQuantity,
    config: SenzorConfiguration,
    created: number,
    edited?: number,
    deleted?: number
}

export default interface MeasurementPoint extends BaseEntity {
    organisationId: ObjectId,
    ownerId: string,
    name: string,
    description: string,
    influxMeasurement: string,
    sensors: Senzor[],
}