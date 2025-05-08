import apiClient from "../axiosInstance"
import { PaginatedRequest, PaginatedResponse } from "../types/basic";
import { SensorConfiguration, MeasurementPoint, MeasuredQuantity, Sensor } from "./measurementPointsRequests"


const URL_PREFIX = "/sensor"

// Add Measurement Point
export interface AddSensorDtoIn {
    measurementPointId: string,
    name: string,
    quantity: MeasuredQuantity,
    config: SensorConfiguration,
}
const addSensor = async (data: AddSensorDtoIn) => {
    const response = await apiClient.post(`${URL_PREFIX}/add`, data);
    return response.data as Sensor[];
};

// Delete Measurement Point
export interface DeleteMeasurementPointDtoIn {
    organisationId: string;
    id: string;
}
const deleteMeasurementPoint = async (data: DeleteMeasurementPointDtoIn) => {
    const response = await apiClient.post(`${URL_PREFIX}/delete`, data);
    return response.status === 202;
};

// Get Measurement Point by ID
const getMeasurementPoint = async (id: string) => {
    const response = await apiClient.get(`${URL_PREFIX}/get/${id}`);
    return response.data as MeasurementPoint;
};

// List Measurement Points
export interface ListMeasurementPointsDtoIn extends PaginatedRequest {
    organisationId: string;
}
export interface ListMeasurementPointsDtoOut extends PaginatedResponse {
    measurementPoints: MeasurementPoint[];
}
const listMeasurementPoints = async (data: ListMeasurementPointsDtoIn) => {
    const response = await apiClient.post(`${URL_PREFIX}/list`, data);
    return response.data as ListMeasurementPointsDtoOut;
};

// Update Measurement Point
export interface UpdateMeasurementPointDtoIn {
    id: string;
    name?: string;
    description?: string;
}
const updateMeasurementPoint = async (data: UpdateMeasurementPointDtoIn) => {
    const response = await apiClient.post(`${URL_PREFIX}/update`, data);
    return response.data as MeasurementPoint;
};

// Get JWT Token for Measurement Point
export interface GetMeasurementPointJwtTokenDtoIn {
    _id: string;
}
export interface GetMeasurementPointJwtTokenDtoOut {
    jwtToken: string;
}
const getMeasurementPointJwtToken = async (data: GetMeasurementPointJwtTokenDtoIn) => {
    const response = await apiClient.get(`${URL_PREFIX}/getJwtToken`, { params: data });
    const responseObject = response.data as GetMeasurementPointJwtTokenDtoOut;
    return responseObject.jwtToken;
};

// Export all requests
const measurementPointsRequests = {
    addSensor,
    deleteMeasurementPoint,
    getMeasurementPoint,
    listMeasurementPoints,
    updateMeasurementPoint,
    getMeasurementPointJwtToken,
};

export default measurementPointsRequests;