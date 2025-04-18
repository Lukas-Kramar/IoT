import { JSONSchemaType } from "ajv";
import { VALIDATION_ERRORS } from "../../errors/errorMessages";
import { Policy } from "../../models/Organisation";
export interface OrganisationUserSchema {
    policy: Policy,
    id: string,
}
export const organisationUserSchema: JSONSchemaType<OrganisationUserSchema> = {
    type: 'object',
    properties: {
        policy: {
            type: 'integer',
            enum: [Policy.Admin, Policy.Member],
            errorMessage: {
                type: `${VALIDATION_ERRORS.TYPE} integer`,
                enum: `${VALIDATION_ERRORS.ENUM} Policy (0 for Admin, 1 for Member)`,
            },
        },
        id: {
            type: 'string',
            format: 'objectId',
            errorMessage: {
                type: `${VALIDATION_ERRORS.TYPE} string`,
                format: `${VALIDATION_ERRORS.FORMAT} ObjectId (Mongo)`,
            },
        },
    },
    required: ['policy', 'id'],
    additionalProperties: false,
}