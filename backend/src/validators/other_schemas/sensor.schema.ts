import { JSONSchemaType } from "ajv";
import { VALIDATION_ERRORS } from "../../errors/errorMessages";
import sensorConfigSchema from "./sensorConfig.schema";
import { MeasuredQuantity, SenzorConfiguration } from "../../models/MeasurementPoint";

export interface CreateSensor {
    sensorId: string,
    name: string,
    quantity: MeasuredQuantity,
    config: SenzorConfiguration,
}

export const createSensorSchema: JSONSchemaType<CreateSensor> = {
    type: 'object',
    properties: {
        sensorId: {
            type: 'string',
            format: 'uuid',
            errorMessage: {
                type: `${VALIDATION_ERRORS.TYPE} String`,
                format: `${VALIDATION_ERRORS.FORMAT} UUID`,
            }
        },
        name: {
            type: 'string',
            minLength: 5,
            errorMessage: {
                minLength: `${VALIDATION_ERRORS.MIN_LENGTH} 5`,
                type: `${VALIDATION_ERRORS.TYPE} String`,
            },
        },
        quantity: {
            type: "string",
            enum: ["temperature", "acceleration"],
            errorMessage: {
                enum: `${VALIDATION_ERRORS.PATTERN} "temperature" or "acceleration"`,
                type: `${VALIDATION_ERRORS.TYPE} String`
            }
        },
        config: sensorConfigSchema,
    },
    required: ['sensorId', 'name', 'quantity', 'config'],
    additionalProperties: false,
};

export interface UpdateSensor {
    sensorId: string,
    name: string,
    quantity: MeasuredQuantity,
    config: SenzorConfiguration,
    created: number,
}
export const updateSensorSchema: JSONSchemaType<UpdateSensor> = {
    type: 'object',
    properties: {
        sensorId: {
            type: 'string',
            format: 'uuid',
            errorMessage: {
                type: `${VALIDATION_ERRORS.TYPE} String`,
                format: `${VALIDATION_ERRORS.FORMAT} UUID`,
            }
        },
        name: {
            type: 'string',
            minLength: 5,
            errorMessage: {
                minLength: `${VALIDATION_ERRORS.MIN_LENGTH} 5`,
                type: `${VALIDATION_ERRORS.TYPE} String`,
            },
        },
        quantity: {
            type: "string",
            enum: ["temperature", "acceleration"],
            errorMessage: {
                enum: `${VALIDATION_ERRORS.PATTERN} "temperature" or "acceleration"`,
                type: `${VALIDATION_ERRORS.TYPE} String`
            }
        },
        created: {
            type: "number",

        },
        config: sensorConfigSchema,
    },
    required: ['sensorId', 'name', 'quantity', 'config', "created"],
    additionalProperties: false,
};