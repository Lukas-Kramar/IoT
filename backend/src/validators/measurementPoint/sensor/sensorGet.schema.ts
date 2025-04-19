
import { ParamsDictionary, Query } from 'express-serve-static-core';
import { ajv, JSONSchemaType, VALIDATION_ERRORS, } from "../../../ajv-validation/ajvInstance"

export interface sensorGetConfigurationParams extends ParamsDictionary {
    sensorId: string,
}
const sensorGetConfigurationParamsScheme: JSONSchemaType<sensorGetConfigurationParams> = {
    type: 'object',
    properties: {
        sensorId: {
            type: 'string',
            format: 'uuid',
            errorMessage: {
                type: `${VALIDATION_ERRORS.TYPE} String`,
                format: `${VALIDATION_ERRORS.FORMAT} UUID`
            },
        },
    },
    required: ['sensorId'],
    additionalProperties: false,
};

export const validateSensorGetConfigurationParams = ajv.compile(sensorGetConfigurationParamsScheme);

export interface sensorGetConfigurationQuery extends Query {
    measurementPointId?: string,
}
const sensorGetConfigurationQueryScheme: JSONSchemaType<sensorGetConfigurationQuery> = {
    type: 'object',
    properties: {
        measurementPointId: {
            type: 'string',
            format: 'uuid',
            nullable: true,
            errorMessage: {
                type: `${VALIDATION_ERRORS.TYPE} String`,
                format: `${VALIDATION_ERRORS.FORMAT} UUID`
            },
        },
    },
    required: [],
    additionalProperties: false,
};
export const validateSensorGetConfigurationQuery = ajv.compile(sensorGetConfigurationQueryScheme);
